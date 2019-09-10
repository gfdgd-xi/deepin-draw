#include "editlabel.h"

#include <QHBoxLayout>
#include <QFontMetrics>
#include <QDebug>

#include "utils/global.h"

const QSize LINEEDIT_SIZE = QSize(37, 24);

EditLabel::EditLabel(DWidget *parent)
    : DLabel(parent)
    , m_titleSpacing(1)
{
    QFont font = this->font();

    font.setPointSize(8);


    m_titleLabel = new DLabel(this);

    m_edit = new DLineEdit(this);
    m_edit->setFixedSize(LINEEDIT_SIZE);
    m_edit->setClearButtonEnabled(false);
    m_edit->setFont(font);


    QHBoxLayout *mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addWidget(m_titleLabel);
    mLayout->addWidget(m_edit);

    connect(m_edit, &DLineEdit::editingFinished, this, [ = ] {
        emit editTextChanged(m_edit->text());
    });

    setLayout(mLayout);
}

void EditLabel::setTitle(QString title)
{
    m_titleLabel->setText(title);
    QFont font = m_titleLabel->font();
    QFontMetrics fm(font);
    m_titleLabel->setFixedWidth(fm.boundingRect(m_titleLabel->text()).width());
}

void EditLabel::setEditText(QString text)
{
    m_edit->setText(text);
}

void EditLabel::setEditWidth(int width)
{
    m_edit->setFixedWidth(width);
    this->updateGeometry();
}

void EditLabel::setTitleSpacing(int spacing)
{
    m_titleSpacing = spacing;
    this->updateGeometry();
}

QString EditLabel::editText()
{
    return m_edit->text();
}

EditLabel::~EditLabel() {}
