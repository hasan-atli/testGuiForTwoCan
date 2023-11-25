#include "QUInt32LineEdit.h"

QUInt32LineEdit::QUInt32LineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    setValidator(new QUInt32Validator(this));

    // İstediğiniz özel işlemleri burada ekleyebilirsiniz.
}

uint32_t QUInt32LineEdit::value() const
{
    return text().toUInt();
}

void QUInt32LineEdit::setValue(uint32_t val)
{
    setText(QString::number(val));
}

QUInt32LineEdit::QUInt32Validator::QUInt32Validator(QObject* parent)
    : QValidator(parent)
{
}

QValidator::State QUInt32LineEdit::QUInt32Validator::validate(QString& input, int& pos) const
{
    bool ok;
    uint32_t val = input.toUInt(&ok);
    if (ok && val <= std::numeric_limits<uint32_t>::max()) {
        return Acceptable;
    } else {
        return Invalid;
    }
}
