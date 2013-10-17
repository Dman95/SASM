#include "watchsettinswidget.h"

WatchSettinsWidget::WatchSettinsWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QHBoxLayout(this);
    typeComboBox = new QComboBox;
    sizeComboBox = new QComboBox;
    arraySizeEdit = new QLineEdit;
    addressCheckbox = new QCheckBox(tr("Address"));
    layout->addWidget(typeComboBox);
    layout->addWidget(sizeComboBox);
    layout->addWidget(arraySizeEdit);
    layout->addWidget(addressCheckbox);

    arraySizeEdit->setPlaceholderText(tr("Array size"));

    QStringList comboBoxList;
    comboBoxList << tr("Smart") << tr("Hex") << tr("Bin") << tr("Char") << tr("Int") << tr("UInt");
    typeComboBox->insertItems(0, comboBoxList);

    QStringList sizeBoxList;
    sizeBoxList << tr("d") << tr("w") << tr("b");
    sizeComboBox->insertItems(0, sizeBoxList);

    addressCheckbox->setChecked(false);

    layout->setSpacing(0);
    layout->setMargin(0);

    connect(typeComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));
    connect(sizeComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));
    connect(arraySizeEdit, SIGNAL(textChanged(QString)), this, SIGNAL(settingsChanged()));
    connect(addressCheckbox, SIGNAL(stateChanged(int)), this, SIGNAL(settingsChanged()));

    setLayout(layout);
}

int WatchSettinsWidget::sumSize()
{
    return typeComboBox->sizeHint().width() + sizeComboBox->sizeHint().width()
            + arraySizeEdit->fontMetrics().width(arraySizeEdit->placeholderText())
            + addressCheckbox->sizeHint().width() + 10;
}

QSize WatchSettinsWidget::sizeHint() const
{
    return QSize(typeComboBox->sizeHint().width() + sizeComboBox->sizeHint().width()
            + arraySizeEdit->fontMetrics().width(arraySizeEdit->placeholderText())
            + addressCheckbox->sizeHint().width() + 10, 0);
}

WatchSettinsWidget::~WatchSettinsWidget()
{
    delete arraySizeEdit;
    delete typeComboBox;
    delete sizeComboBox;
    delete addressCheckbox;
    delete layout;
}
