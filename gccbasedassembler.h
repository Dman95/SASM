#ifndef GCCBASEDASSEMBLER_H
#define GCCBASEDASSEMBLER_H
#include "assembler.h"

class GccBasedAssembler : public Assembler
{
public:
    explicit GccBasedAssembler(bool x86, QObject *parent = 0);
    QString getLinkerPath() override;
    QString getLinkerOptions() override;
private:
    bool m_isPieEnabled;
};

#endif // GCCBASEDASSEMBLER_H
