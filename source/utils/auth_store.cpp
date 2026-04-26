#include "utils/auth_store.h"

#include "log.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <direct.h>
#include <process.h>
#include <windows.h>
#else
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

constexpr int kSaltSize = 16;
constexpr int kIvSize = 12;
constexpr int kTagSize = 16;
constexpr int kKeySize = 32;
constexpr int kPbkdf2Iterations = 100000;

std::string hex_encode(const unsigned char *data, size_t size) {
    static const char digits[] = "0123456789abcdef";
    std::string out;
    out.reserve(size * 2);
    for (size_t i = 0; i < size; ++i) {
        out.push_back(digits[(data[i] >> 4) & 0x0f]);
        out.push_back(digits[data[i] & 0x0f]);
    }
    return out;
}

int hex_value(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

bool hex_decode(const std::string &hex, std::vector<unsigned char> &out) {
    if ((hex.size() % 2) != 0) {
        return false;
    }
    out.clear();
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        const int high = hex_value(hex[i]);
        const int low = hex_value(hex[i + 1]);
        if (high < 0 || low < 0) {
            return false;
        }
        out.push_back(static_cast<unsigned char>((high << 4) | low));
    }
    return true;
}

std::string sha256_hex(const std::string &value) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(value.data()), value.size(), digest);
    return hex_encode(digest, sizeof(digest));
}

std::string env_value(const char *name) {
    const char *value = std::getenv(name);
    return value ? std::string(value) : std::string();
}

std::string current_hostname() {
#if defined(_WIN32)
    char buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(buffer);
    return GetComputerNameA(buffer, &size) ? std::string(buffer, size) : std::string();
#else
    char buffer[256];
    return gethostname(buffer, sizeof(buffer)) == 0 ? std::string(buffer) : std::string();
#endif
}

std::string current_user_marker() {
#if defined(_WIN32)
    return env_value("USERNAME") + ":" + env_value("USERPROFILE");
#else
    std::string marker = std::to_string(static_cast<long long>(getuid())) + ":";
    if (const passwd *pw = getpwuid(getuid())) {
        marker += pw->pw_name;
    }
    marker += ":" + env_value("HOME");
    return marker;
#endif
}

std::string machine_secret(const std::string &port, const std::string &user) {
    return "p5-auto-login-v1\n" + port + "\n" + user + "\n" + current_user_marker() + "\n" + current_hostname();
}

bool derive_key(const std::string &port, const std::string &user, const std::vector<unsigned char> &salt, unsigned char key[kKeySize]) {
    const std::string secret = machine_secret(port, user);
    return PKCS5_PBKDF2_HMAC(secret.c_str(),
                             static_cast<int>(secret.size()),
                             salt.data(),
                             static_cast<int>(salt.size()),
                             kPbkdf2Iterations,
                             EVP_sha256(),
                             kKeySize,
                             key) == 1;
}

std::string store_dir() {
    if (const char *override_dir = std::getenv("P5_PASSWORD_STORE")) {
        if (override_dir[0] != '\0') {
            return std::string(override_dir);
        }
    }
#if defined(_WIN32)
    std::string base = env_value("APPDATA");
    if (base.empty()) {
        base = env_value("USERPROFILE");
    }
    return base.empty() ? std::string() : base + "\\p5";
#else
    const std::string home = env_value("HOME");
    return home.empty() ? std::string() : home + "/.p5";
#endif
}

std::string store_path(const std::string &port, const std::string &user) {
    const std::string dir = store_dir();
    if (dir.empty()) {
        return std::string();
    }
#if defined(_WIN32)
    return dir + "\\" + sha256_hex(port + "\n" + user) + ".credential";
#else
    return dir + "/" + sha256_hex(port + "\n" + user) + ".credential";
#endif
}

bool ensure_store_dir(const std::string &dir) {
    if (dir.empty()) {
        return false;
    }
#if defined(_WIN32)
    if (_mkdir(dir.c_str()) == 0 || errno == EEXIST) {
        return true;
    }
#else
    if (mkdir(dir.c_str(), 0700) == 0 || errno == EEXIST) {
        chmod(dir.c_str(), 0700);
        return true;
    }
#endif
    ERROR("p5 login: could not create password store " << dir << ": " << std::strerror(errno));
    return false;
}

bool encrypt_password(const std::string &port,
                      const std::string &user,
                      const std::string &password,
                      std::vector<unsigned char> &salt,
                      std::vector<unsigned char> &iv,
                      std::vector<unsigned char> &tag,
                      std::vector<unsigned char> &ciphertext) {
    salt.assign(kSaltSize, 0);
    iv.assign(kIvSize, 0);
    tag.assign(kTagSize, 0);
    ciphertext.assign(password.size(), 0);
    if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1 || RAND_bytes(iv.data(), static_cast<int>(iv.size())) != 1) {
        ERROR("p5 login: could not generate encryption randomness");
        return false;
    }

    unsigned char key[kKeySize];
    if (!derive_key(port, user, salt, key)) {
        ERROR("p5 login: could not derive encryption key");
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        OPENSSL_cleanse(key, sizeof(key));
        return false;
    }

    int len = 0;
    int out_len = 0;
    const std::string aad = port + "\n" + user;
    bool ok = EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) == 1 &&
              EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(iv.size()), nullptr) == 1 &&
              EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv.data()) == 1 &&
              EVP_EncryptUpdate(ctx, nullptr, &len, reinterpret_cast<const unsigned char *>(aad.data()), static_cast<int>(aad.size())) == 1 &&
              EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char *>(password.data()), static_cast<int>(password.size())) == 1;
    out_len = len;
    ok = ok && EVP_EncryptFinal_ex(ctx, ciphertext.data() + out_len, &len) == 1;
    out_len += len;
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(tag.size()), tag.data()) == 1;
    ciphertext.resize(static_cast<size_t>(out_len));
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key, sizeof(key));
    return ok;
}

bool decrypt_password(const std::string &port,
                      const std::string &user,
                      const std::vector<unsigned char> &salt,
                      const std::vector<unsigned char> &iv,
                      const std::vector<unsigned char> &tag,
                      const std::vector<unsigned char> &ciphertext,
                      std::string &password) {
    if (salt.size() != kSaltSize || iv.size() != kIvSize || tag.size() != kTagSize) {
        return false;
    }

    unsigned char key[kKeySize];
    if (!derive_key(port, user, salt, key)) {
        return false;
    }

    std::vector<unsigned char> plaintext(ciphertext.size());
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        OPENSSL_cleanse(key, sizeof(key));
        return false;
    }

    int len = 0;
    int out_len = 0;
    const std::string aad = port + "\n" + user;
    bool ok = EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) == 1 &&
              EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(iv.size()), nullptr) == 1 &&
              EVP_DecryptInit_ex(ctx, nullptr, nullptr, key, iv.data()) == 1 &&
              EVP_DecryptUpdate(ctx, nullptr, &len, reinterpret_cast<const unsigned char *>(aad.data()), static_cast<int>(aad.size())) == 1 &&
              EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), static_cast<int>(ciphertext.size())) == 1;
    out_len = len;
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(tag.size()), const_cast<unsigned char *>(tag.data())) == 1 &&
         EVP_DecryptFinal_ex(ctx, plaintext.data() + out_len, &len) == 1;
    out_len += len;
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key, sizeof(key));
    if (!ok) {
        return false;
    }
    password.assign(reinterpret_cast<const char *>(plaintext.data()), static_cast<size_t>(out_len));
    OPENSSL_cleanse(plaintext.data(), plaintext.size());
    return true;
}

} // namespace

bool AuthStore::SavePassword(const std::string &port, const std::string &user, const std::string &password) {
    const std::string dir = store_dir();
    const std::string path = store_path(port, user);
    if (dir.empty() || path.empty()) {
        ERROR("p5 login: HOME/APPDATA is not set; cannot save password");
        return false;
    }
    if (!ensure_store_dir(dir)) {
        return false;
    }

    std::vector<unsigned char> salt;
    std::vector<unsigned char> iv;
    std::vector<unsigned char> tag;
    std::vector<unsigned char> ciphertext;
    if (!encrypt_password(port, user, password, salt, iv, tag, ciphertext)) {
        ERROR("p5 login: could not encrypt password");
        return false;
    }

    const std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp.c_str(), std::ios::binary);
        if (!out) {
            ERROR("p5 login: could not write password store " << tmp);
            return false;
        }
        out << "p5-auth-v1\n"
            << hex_encode(salt.data(), salt.size()) << "\n"
            << hex_encode(iv.data(), iv.size()) << "\n"
            << hex_encode(tag.data(), tag.size()) << "\n"
            << hex_encode(ciphertext.data(), ciphertext.size()) << "\n";
    }
    if (std::rename(tmp.c_str(), path.c_str()) != 0) {
        ERROR("p5 login: could not update password store " << path << ": " << std::strerror(errno));
        (void)std::remove(tmp.c_str());
        return false;
    }
#if !defined(_WIN32)
    chmod(path.c_str(), 0600);
#endif
    return true;
}

bool AuthStore::LoadPassword(const std::string &port, const std::string &user, std::string &password) {
    const std::string path = store_path(port, user);
    if (path.empty()) {
        return false;
    }

    std::ifstream in(path.c_str(), std::ios::binary);
    if (!in) {
        return false;
    }

    std::string version;
    std::string salt_hex;
    std::string iv_hex;
    std::string tag_hex;
    std::string ciphertext_hex;
    if (!std::getline(in, version) || !std::getline(in, salt_hex) || !std::getline(in, iv_hex) || !std::getline(in, tag_hex) || !std::getline(in, ciphertext_hex)) {
        return false;
    }
    if (version != "p5-auth-v1") {
        return false;
    }

    std::vector<unsigned char> salt;
    std::vector<unsigned char> iv;
    std::vector<unsigned char> tag;
    std::vector<unsigned char> ciphertext;
    if (!hex_decode(salt_hex, salt) || !hex_decode(iv_hex, iv) || !hex_decode(tag_hex, tag) || !hex_decode(ciphertext_hex, ciphertext)) {
        return false;
    }
    return decrypt_password(port, user, salt, iv, tag, ciphertext, password);
}
