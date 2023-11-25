#ifndef QUINT32LINEEDIT_H
#define QUINT32LINEEDIT_H

#include <QLineEdit>
#include <QValidator>
#include <limits>

class QUInt32LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    QUInt32LineEdit(QWidget* parent = nullptr);

    uint32_t value() const;
    void setValue(uint32_t val);

private:
    class QUInt32Validator : public QValidator
    {
    public:
        QUInt32Validator(QObject* parent = nullptr);

        virtual State validate(QString& input, int& pos) const override;
    };
};

#endif // QUINT32LINEEDIT_H
