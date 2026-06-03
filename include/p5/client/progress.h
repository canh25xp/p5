#pragma once

#include <p4/clientapi.h>
#include <p4/clientprog.h>

#include <string>

/// P4Python Progress base class.
class Progress {
public:
    static constexpr int TYPE_SENDFILE = 1;
    static constexpr int TYPE_RECEIVEFILE = 2;
    static constexpr int TYPE_TRANSFER = 3;
    static constexpr int TYPE_COMPUTATION = 4;

    static constexpr int UNIT_PERCENT = 1;
    static constexpr int UNIT_FILES = 2;
    static constexpr int UNIT_KBYTES = 3;
    static constexpr int UNIT_MBYTES = 4;

    virtual ~Progress() = default;

    virtual void init(int type);
    virtual void setDescription(const std::string &description, int units);
    virtual void setTotal(long total);
    virtual void update(long position);
    virtual void done(int fail);

protected:
    int m_type{0};
    std::string m_description;
    int m_units{0};
    long m_total{0};
    long m_position{0};
};

/// Simple stderr progress logger (P4Python TextProgress).
class TextProgress : public Progress {
public:
    void init(int type) override;
    void setDescription(const std::string &description, int units) override;
    void setTotal(long total) override;
    void update(long position) override;
    void done(int fail) override;

private:
    static const char *TypeName(int type);
    static const char *UnitName(int units);
};

/// Bridges p5::Progress to P4API ClientProgress.
class ProgressClientAdapter : public ClientProgress {
public:
    explicit ProgressClientAdapter(Progress *progress) : m_progress(progress) {}

    void Description(const StrPtr *desc, int units) override;
    void Total(long total) override;
    int Update(long position) override;
    void Done(int fail) override;

private:
    Progress *m_progress;
};
