#include "client/progress.h"

#include <iostream>

void Progress::init(int type) {
    m_type = type;
}

void Progress::setDescription(const std::string &description, int units) {
    m_description = description;
    m_units = units;
}

void Progress::setTotal(long total) {
    m_total = total;
}

void Progress::update(long position) {
    m_position = position;
}

void Progress::done(int /*fail*/) {}

const char *TextProgress::TypeName(int type) {
    static const char *kTypes[] = {"Unknown", "Submit", "Sync", "Clone"};
    if (type >= 0 && type < 4) {
        return kTypes[type];
    }
    return "Unknown";
}

const char *TextProgress::UnitName(int units) {
    static const char *kUnits[] = {"Unknown", "Percent", "Files", "KBytes", "MBytes"};
    if (units >= 0 && units < 5) {
        return kUnits[units];
    }
    return "Unknown";
}

void TextProgress::init(int type) {
    Progress::init(type);
    std::cerr << "Progress.init with '" << TypeName(type) << "'\n";
}

void TextProgress::setDescription(const std::string &description, int units) {
    Progress::setDescription(description, units);
    std::cerr << "Progress.setDescription with '" << description << "' and units '" << UnitName(units) << "'\n";
}

void TextProgress::setTotal(long total) {
    Progress::setTotal(total);
    std::cerr << "Progress.setTotal with '" << total << "'\n";
}

void TextProgress::update(long position) {
    Progress::update(position);
    std::cerr << "Progress.update with '" << position << "'\n";
}

void TextProgress::done(int fail) {
    Progress::done(fail);
    std::cerr << "Progress.done with '" << fail << "'\n";
}

void ProgressClientAdapter::Description(const StrPtr *desc, int units) {
    if (m_progress && desc) {
        m_progress->setDescription(desc->Text(), units);
    }
}

void ProgressClientAdapter::Total(long total) {
    if (m_progress) {
        m_progress->setTotal(total);
    }
}

int ProgressClientAdapter::Update(long position) {
    if (m_progress) {
        m_progress->update(position);
    }
    return 0;
}

void ProgressClientAdapter::Done(int fail) {
    if (m_progress) {
        m_progress->done(fail);
    }
}
