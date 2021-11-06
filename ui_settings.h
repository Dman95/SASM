/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *settingsLabel;
    QTabWidget *tabWidget;
    QWidget *commonTab;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *commonSettingsBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *startWindowLabel;
    QComboBox *startWindow;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *languageLabel;
    QComboBox *language;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *registersLabel_2;
    QRadioButton *registersYesRadioButton;
    QRadioButton *registersNoRadioButton;
    QSpacerItem *horizontalSpacer_9;
    QHBoxLayout *horizontalLayout_7;
    QLabel *insertDebugStringLabel;
    QCheckBox *insertDebugStringCheckBox;
    QSpacerItem *horizontalSpacer_10;
    QGroupBox *codeSettingsBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *fontLabel;
    QFontComboBox *fontComboBox;
    QLabel *fontSizeLabel;
    QSpinBox *fontSizeSpinBox;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label;
    QLabel *label_2;
    QWidget *startTextWidget;
    QSpacerItem *verticalSpacer_5;
    QHBoxLayout *horizontalLayout;
    QToolButton *resetAllButton;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer;
    QWidget *colorsTab;
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QCheckBox *iomacroBoldCheckBox;
    QLabel *systemLabel;
    QPushButton *quotationColorButton;
    QPushButton *systemColorButton;
    QCheckBox *numbersItalicCheckBox;
    QLabel *label_6;
    QCheckBox *numbersBoldCheckBox;
    QLabel *label_7;
    QLabel *label_3;
    QCheckBox *systemItalicCheckBox;
    QPushButton *commentsColorButton_2;
    QPushButton *keywordsColorButton;
    QCheckBox *commentsBoldCheckBox;
    QPushButton *keywordsColorButton_2;
    QPushButton *iomacroColorButton_2;
    QCheckBox *memoryItalicCheckBox;
    QLabel *keywordsLabel;
    QLabel *label_5;
    QPushButton *labelsColorButton;
    QLabel *numbersLabel;
    QPushButton *quotationColorButton_2;
    QLabel *labelsLabel;
    QLabel *commentsLabel;
    QPushButton *memoryColorButton;
    QPushButton *systemColorButton_2;
    QLabel *iomacroLabel_2;
    QPushButton *labelsColorButton_2;
    QCheckBox *registersBoldCheckBox;
    QCheckBox *quotationBoldCheckBox;
    QPushButton *iomacroColorButton;
    QCheckBox *iomacroItalicCheckBox;
    QPushButton *registersColorButton_2;
    QCheckBox *keywordsBoldCheckBox;
    QCheckBox *memoryBoldCheckBox;
    QPushButton *numbersColorButton;
    QCheckBox *quotationItalicCheckBox;
    QPushButton *commentsColorButton;
    QCheckBox *labelsItalicCheckBox;
    QLabel *memoryLabel;
    QLabel *iomacroLabel;
    QCheckBox *keywordsItalicCheckBox;
    QSpacerItem *verticalSpacer_3;
    QLabel *registersLabel;
    QCheckBox *commentsItalicCheckBox;
    QCheckBox *labelsBoldCheckBox;
    QCheckBox *registersItalicCheckBox;
    QCheckBox *systemBoldCheckBox;
    QPushButton *memoryColorButton_2;
    QPushButton *registersColorButton;
    QPushButton *numbersColorButton_2;
    QSpacerItem *horizontalSpacer_7;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_5;
    QGridLayout *gridLayout_4;
    QLabel *fontLabel_2;
    QPushButton *fontColorButton;
    QCheckBox *currentLineCheckBox;
    QPushButton *debugLineColorButton;
    QLabel *currentLineLabel;
    QLabel *debugLineLabel;
    QPushButton *currentLineColorButton;
    QLabel *backgroundLabel;
    QPushButton *backgroundColorButton;
    QLabel *lineNumberPanelLabel;
    QPushButton *lineNumberPanelColorButton;
    QLabel *lineNumberFontLabel;
    QPushButton *lineNumberFontColorButton;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *verticalSpacer_4;
    QWidget *buildTab;
    QVBoxLayout *verticalLayout_6;
    QFormLayout *formLayout;
    QLabel *modeLabel;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *x86RadioButton;
    QRadioButton *x64RadioButton;
    QSpacerItem *horizontalSpacer;
    QLabel *assemblerLabel;
    QHBoxLayout *horizontalLayout_6;
    QRadioButton *nasmRadioButton;
    QRadioButton *gasRadioButton;
    QRadioButton *fasmRadioButton;
    QRadioButton *masmRadioButton;
    QSpacerItem *horizontalSpacer_8;
    QLabel *assemblyLabel;
    QLineEdit *assemblyOptionsEdit;
    QLabel *linkingLabel;
    QLineEdit *linkingOptionsEdit;
    QLabel *assemblerPathLabel;
    QLineEdit *assemblerPathEdit;
    QLabel *linkerPathLabel;
    QLineEdit *linkerPathEdit;
    QLabel *objectFileNameLabel;
    QLineEdit *objectFileNameEdit;
    QLabel *gdbPathLabel;
    QLineEdit *gdbPathEdit;
    QLabel *assemblerWorkingDirectoryLabel;
    QCheckBox *runInCurrentDirectoryCheckbox;
    QLabel *disableLinkingLabel;
    QCheckBox *disableLinkingCheckbox;
    QLabel *gdbVerboseLabel;
    QCheckBox *sasmVerboseCheckBox;
    QLabel *MiModusLabel;
    QCheckBox *MiModusCheckBox;
    QLabel *gdbDisplayLabel;
    QCheckBox *sasmDisplayCheckBox;
    QLabel *infoLabel;
    QDialogButtonBox *buttonBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *buttonGroup_2;
    QButtonGroup *buttonGroup_3;

    void setupUi(QWidget *SettingsWindow)
    {
        if (SettingsWindow->objectName().isEmpty())
            SettingsWindow->setObjectName(QString::fromUtf8("SettingsWindow"));
        SettingsWindow->resize(1702, 971);
        verticalLayout = new QVBoxLayout(SettingsWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        settingsLabel = new QLabel(SettingsWindow);
        settingsLabel->setObjectName(QString::fromUtf8("settingsLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(settingsLabel->sizePolicy().hasHeightForWidth());
        settingsLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(16);
        settingsLabel->setFont(font);
        settingsLabel->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        verticalLayout->addWidget(settingsLabel);

        tabWidget = new QTabWidget(SettingsWindow);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setLayoutDirection(Qt::LeftToRight);
        tabWidget->setElideMode(Qt::ElideNone);
        commonTab = new QWidget();
        commonTab->setObjectName(QString::fromUtf8("commonTab"));
        verticalLayout_4 = new QVBoxLayout(commonTab);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        commonSettingsBox = new QGroupBox(commonTab);
        commonSettingsBox->setObjectName(QString::fromUtf8("commonSettingsBox"));
        sizePolicy.setHeightForWidth(commonSettingsBox->sizePolicy().hasHeightForWidth());
        commonSettingsBox->setSizePolicy(sizePolicy);
        commonSettingsBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_3 = new QVBoxLayout(commonSettingsBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        startWindowLabel = new QLabel(commonSettingsBox);
        startWindowLabel->setObjectName(QString::fromUtf8("startWindowLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(startWindowLabel->sizePolicy().hasHeightForWidth());
        startWindowLabel->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(startWindowLabel);

        startWindow = new QComboBox(commonSettingsBox);
        startWindow->addItem(QString());
        startWindow->addItem(QString());
        startWindow->setObjectName(QString::fromUtf8("startWindow"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(startWindow->sizePolicy().hasHeightForWidth());
        startWindow->setSizePolicy(sizePolicy2);
        startWindow->setMinimumSize(QSize(266, 0));

        horizontalLayout_2->addWidget(startWindow);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        languageLabel = new QLabel(commonSettingsBox);
        languageLabel->setObjectName(QString::fromUtf8("languageLabel"));

        horizontalLayout_4->addWidget(languageLabel);

        language = new QComboBox(commonSettingsBox);
        language->addItem(QString::fromUtf8("\320\240\321\203\321\201\321\201\320\272\320\270\320\271"));
        language->addItem(QString::fromUtf8("English"));
        language->addItem(QString::fromUtf8("T\303\274rk"));
        language->addItem(QString::fromUtf8("\344\270\255\345\233\275"));
        language->addItem(QString::fromUtf8("Deutsch"));
        language->addItem(QString::fromUtf8("Italiano"));
        language->addItem(QString::fromUtf8("Polski"));
        language->addItem(QString::fromUtf8("\327\242\327\221\327\250\327\231\327\252"));
        language->addItem(QString::fromUtf8("Espa\303\261ol"));
        language->addItem(QString::fromUtf8("Portugu\303\252s"));
        language->setObjectName(QString::fromUtf8("language"));

        horizontalLayout_4->addWidget(language);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout_3->addLayout(horizontalLayout_4);

        label_4 = new QLabel(commonSettingsBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_3->addWidget(label_4);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        registersLabel_2 = new QLabel(commonSettingsBox);
        registersLabel_2->setObjectName(QString::fromUtf8("registersLabel_2"));

        horizontalLayout_8->addWidget(registersLabel_2);

        registersYesRadioButton = new QRadioButton(commonSettingsBox);
        buttonGroup_3 = new QButtonGroup(SettingsWindow);
        buttonGroup_3->setObjectName(QString::fromUtf8("buttonGroup_3"));
        buttonGroup_3->addButton(registersYesRadioButton);
        registersYesRadioButton->setObjectName(QString::fromUtf8("registersYesRadioButton"));
        registersYesRadioButton->setChecked(false);

        horizontalLayout_8->addWidget(registersYesRadioButton);

        registersNoRadioButton = new QRadioButton(commonSettingsBox);
        buttonGroup_3->addButton(registersNoRadioButton);
        registersNoRadioButton->setObjectName(QString::fromUtf8("registersNoRadioButton"));
        registersNoRadioButton->setChecked(true);

        horizontalLayout_8->addWidget(registersNoRadioButton);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_9);


        verticalLayout_3->addLayout(horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        insertDebugStringLabel = new QLabel(commonSettingsBox);
        insertDebugStringLabel->setObjectName(QString::fromUtf8("insertDebugStringLabel"));

        horizontalLayout_7->addWidget(insertDebugStringLabel);

        insertDebugStringCheckBox = new QCheckBox(commonSettingsBox);
        insertDebugStringCheckBox->setObjectName(QString::fromUtf8("insertDebugStringCheckBox"));
        insertDebugStringCheckBox->setChecked(true);

        horizontalLayout_7->addWidget(insertDebugStringCheckBox);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_10);


        verticalLayout_3->addLayout(horizontalLayout_7);


        verticalLayout_4->addWidget(commonSettingsBox);

        codeSettingsBox = new QGroupBox(commonTab);
        codeSettingsBox->setObjectName(QString::fromUtf8("codeSettingsBox"));
        sizePolicy.setHeightForWidth(codeSettingsBox->sizePolicy().hasHeightForWidth());
        codeSettingsBox->setSizePolicy(sizePolicy);
        codeSettingsBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_2 = new QVBoxLayout(codeSettingsBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        fontLabel = new QLabel(codeSettingsBox);
        fontLabel->setObjectName(QString::fromUtf8("fontLabel"));

        horizontalLayout_5->addWidget(fontLabel);

        fontComboBox = new QFontComboBox(codeSettingsBox);
        fontComboBox->setObjectName(QString::fromUtf8("fontComboBox"));
        fontComboBox->setWritingSystem(QFontDatabase::Latin);
        fontComboBox->setFontFilters(QFontComboBox::AllFonts);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(12);
        fontComboBox->setCurrentFont(font1);

        horizontalLayout_5->addWidget(fontComboBox);

        fontSizeLabel = new QLabel(codeSettingsBox);
        fontSizeLabel->setObjectName(QString::fromUtf8("fontSizeLabel"));

        horizontalLayout_5->addWidget(fontSizeLabel);

        fontSizeSpinBox = new QSpinBox(codeSettingsBox);
        fontSizeSpinBox->setObjectName(QString::fromUtf8("fontSizeSpinBox"));
        fontSizeSpinBox->setMinimum(5);
        fontSizeSpinBox->setMaximum(72);
        fontSizeSpinBox->setValue(12);

        horizontalLayout_5->addWidget(fontSizeSpinBox);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout_2->addLayout(horizontalLayout_5);

        label = new QLabel(codeSettingsBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(codeSettingsBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        verticalLayout_2->addWidget(label_2);

        startTextWidget = new QWidget(codeSettingsBox);
        startTextWidget->setObjectName(QString::fromUtf8("startTextWidget"));

        verticalLayout_2->addWidget(startTextWidget);

        verticalSpacer_5 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_5);


        verticalLayout_4->addWidget(codeSettingsBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        resetAllButton = new QToolButton(commonTab);
        resetAllButton->setObjectName(QString::fromUtf8("resetAllButton"));
        resetAllButton->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        horizontalLayout->addWidget(resetAllButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout_4->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        tabWidget->addTab(commonTab, QString());
        colorsTab = new QWidget();
        colorsTab->setObjectName(QString::fromUtf8("colorsTab"));
        gridLayout = new QGridLayout(colorsTab);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(colorsTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        iomacroBoldCheckBox = new QCheckBox(groupBox);
        iomacroBoldCheckBox->setObjectName(QString::fromUtf8("iomacroBoldCheckBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(iomacroBoldCheckBox->sizePolicy().hasHeightForWidth());
        iomacroBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(iomacroBoldCheckBox, 8, 3, 1, 1);

        systemLabel = new QLabel(groupBox);
        systemLabel->setObjectName(QString::fromUtf8("systemLabel"));

        gridLayout_2->addWidget(systemLabel, 7, 0, 1, 1);

        quotationColorButton = new QPushButton(groupBox);
        quotationColorButton->setObjectName(QString::fromUtf8("quotationColorButton"));
        sizePolicy1.setHeightForWidth(quotationColorButton->sizePolicy().hasHeightForWidth());
        quotationColorButton->setSizePolicy(sizePolicy1);
        quotationColorButton->setMinimumSize(QSize(23, 23));
        quotationColorButton->setMaximumSize(QSize(23, 23));
        quotationColorButton->setBaseSize(QSize(0, 0));
        quotationColorButton->setFocusPolicy(Qt::NoFocus);
        quotationColorButton->setAutoDefault(false);
        quotationColorButton->setFlat(false);

        gridLayout_2->addWidget(quotationColorButton, 9, 1, 1, 1);

        systemColorButton = new QPushButton(groupBox);
        systemColorButton->setObjectName(QString::fromUtf8("systemColorButton"));
        sizePolicy1.setHeightForWidth(systemColorButton->sizePolicy().hasHeightForWidth());
        systemColorButton->setSizePolicy(sizePolicy1);
        systemColorButton->setMinimumSize(QSize(23, 23));
        systemColorButton->setMaximumSize(QSize(23, 23));
        systemColorButton->setBaseSize(QSize(0, 0));
        systemColorButton->setFocusPolicy(Qt::NoFocus);
        systemColorButton->setAutoDefault(false);
        systemColorButton->setFlat(false);

        gridLayout_2->addWidget(systemColorButton, 7, 1, 1, 1);

        numbersItalicCheckBox = new QCheckBox(groupBox);
        numbersItalicCheckBox->setObjectName(QString::fromUtf8("numbersItalicCheckBox"));
        sizePolicy3.setHeightForWidth(numbersItalicCheckBox->sizePolicy().hasHeightForWidth());
        numbersItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(numbersItalicCheckBox, 3, 4, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 0, 3, 1, 1);

        numbersBoldCheckBox = new QCheckBox(groupBox);
        numbersBoldCheckBox->setObjectName(QString::fromUtf8("numbersBoldCheckBox"));
        sizePolicy3.setHeightForWidth(numbersBoldCheckBox->sizePolicy().hasHeightForWidth());
        numbersBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(numbersBoldCheckBox, 3, 3, 1, 1);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 0, 4, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 0, 1, 1, 1);

        systemItalicCheckBox = new QCheckBox(groupBox);
        systemItalicCheckBox->setObjectName(QString::fromUtf8("systemItalicCheckBox"));
        sizePolicy3.setHeightForWidth(systemItalicCheckBox->sizePolicy().hasHeightForWidth());
        systemItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(systemItalicCheckBox, 7, 4, 1, 1);

        commentsColorButton_2 = new QPushButton(groupBox);
        commentsColorButton_2->setObjectName(QString::fromUtf8("commentsColorButton_2"));
        sizePolicy1.setHeightForWidth(commentsColorButton_2->sizePolicy().hasHeightForWidth());
        commentsColorButton_2->setSizePolicy(sizePolicy1);
        commentsColorButton_2->setMinimumSize(QSize(23, 23));
        commentsColorButton_2->setMaximumSize(QSize(23, 23));
        commentsColorButton_2->setBaseSize(QSize(0, 0));
        commentsColorButton_2->setFocusPolicy(Qt::NoFocus);
        commentsColorButton_2->setAutoDefault(false);
        commentsColorButton_2->setFlat(false);

        gridLayout_2->addWidget(commentsColorButton_2, 6, 2, 1, 1);

        keywordsColorButton = new QPushButton(groupBox);
        keywordsColorButton->setObjectName(QString::fromUtf8("keywordsColorButton"));
        sizePolicy1.setHeightForWidth(keywordsColorButton->sizePolicy().hasHeightForWidth());
        keywordsColorButton->setSizePolicy(sizePolicy1);
        keywordsColorButton->setMinimumSize(QSize(23, 23));
        keywordsColorButton->setMaximumSize(QSize(23, 23));
        keywordsColorButton->setBaseSize(QSize(0, 0));
        keywordsColorButton->setFocusPolicy(Qt::NoFocus);
        keywordsColorButton->setAutoDefault(false);
        keywordsColorButton->setFlat(false);

        gridLayout_2->addWidget(keywordsColorButton, 1, 1, 1, 1);

        commentsBoldCheckBox = new QCheckBox(groupBox);
        commentsBoldCheckBox->setObjectName(QString::fromUtf8("commentsBoldCheckBox"));
        sizePolicy3.setHeightForWidth(commentsBoldCheckBox->sizePolicy().hasHeightForWidth());
        commentsBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(commentsBoldCheckBox, 6, 3, 1, 1);

        keywordsColorButton_2 = new QPushButton(groupBox);
        keywordsColorButton_2->setObjectName(QString::fromUtf8("keywordsColorButton_2"));
        sizePolicy1.setHeightForWidth(keywordsColorButton_2->sizePolicy().hasHeightForWidth());
        keywordsColorButton_2->setSizePolicy(sizePolicy1);
        keywordsColorButton_2->setMinimumSize(QSize(23, 23));
        keywordsColorButton_2->setMaximumSize(QSize(23, 23));
        keywordsColorButton_2->setBaseSize(QSize(0, 0));
        keywordsColorButton_2->setFocusPolicy(Qt::NoFocus);
        keywordsColorButton_2->setAutoDefault(false);
        keywordsColorButton_2->setFlat(false);

        gridLayout_2->addWidget(keywordsColorButton_2, 1, 2, 1, 1);

        iomacroColorButton_2 = new QPushButton(groupBox);
        iomacroColorButton_2->setObjectName(QString::fromUtf8("iomacroColorButton_2"));
        sizePolicy1.setHeightForWidth(iomacroColorButton_2->sizePolicy().hasHeightForWidth());
        iomacroColorButton_2->setSizePolicy(sizePolicy1);
        iomacroColorButton_2->setMinimumSize(QSize(23, 23));
        iomacroColorButton_2->setMaximumSize(QSize(23, 23));
        iomacroColorButton_2->setBaseSize(QSize(0, 0));
        iomacroColorButton_2->setFocusPolicy(Qt::NoFocus);
        iomacroColorButton_2->setAutoDefault(false);
        iomacroColorButton_2->setFlat(false);

        gridLayout_2->addWidget(iomacroColorButton_2, 8, 2, 1, 1);

        memoryItalicCheckBox = new QCheckBox(groupBox);
        memoryItalicCheckBox->setObjectName(QString::fromUtf8("memoryItalicCheckBox"));
        sizePolicy3.setHeightForWidth(memoryItalicCheckBox->sizePolicy().hasHeightForWidth());
        memoryItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(memoryItalicCheckBox, 4, 4, 1, 1);

        keywordsLabel = new QLabel(groupBox);
        keywordsLabel->setObjectName(QString::fromUtf8("keywordsLabel"));

        gridLayout_2->addWidget(keywordsLabel, 1, 0, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 0, 2, 1, 1);

        labelsColorButton = new QPushButton(groupBox);
        labelsColorButton->setObjectName(QString::fromUtf8("labelsColorButton"));
        sizePolicy1.setHeightForWidth(labelsColorButton->sizePolicy().hasHeightForWidth());
        labelsColorButton->setSizePolicy(sizePolicy1);
        labelsColorButton->setMinimumSize(QSize(23, 23));
        labelsColorButton->setMaximumSize(QSize(23, 23));
        labelsColorButton->setBaseSize(QSize(0, 0));
        labelsColorButton->setFocusPolicy(Qt::NoFocus);
        labelsColorButton->setAutoDefault(false);
        labelsColorButton->setFlat(false);

        gridLayout_2->addWidget(labelsColorButton, 5, 1, 1, 1);

        numbersLabel = new QLabel(groupBox);
        numbersLabel->setObjectName(QString::fromUtf8("numbersLabel"));

        gridLayout_2->addWidget(numbersLabel, 3, 0, 1, 1);

        quotationColorButton_2 = new QPushButton(groupBox);
        quotationColorButton_2->setObjectName(QString::fromUtf8("quotationColorButton_2"));
        sizePolicy1.setHeightForWidth(quotationColorButton_2->sizePolicy().hasHeightForWidth());
        quotationColorButton_2->setSizePolicy(sizePolicy1);
        quotationColorButton_2->setMinimumSize(QSize(23, 23));
        quotationColorButton_2->setMaximumSize(QSize(23, 23));
        quotationColorButton_2->setBaseSize(QSize(0, 0));
        quotationColorButton_2->setFocusPolicy(Qt::NoFocus);
        quotationColorButton_2->setAutoDefault(false);
        quotationColorButton_2->setFlat(false);

        gridLayout_2->addWidget(quotationColorButton_2, 9, 2, 1, 1);

        labelsLabel = new QLabel(groupBox);
        labelsLabel->setObjectName(QString::fromUtf8("labelsLabel"));

        gridLayout_2->addWidget(labelsLabel, 5, 0, 1, 1);

        commentsLabel = new QLabel(groupBox);
        commentsLabel->setObjectName(QString::fromUtf8("commentsLabel"));

        gridLayout_2->addWidget(commentsLabel, 6, 0, 1, 1);

        memoryColorButton = new QPushButton(groupBox);
        memoryColorButton->setObjectName(QString::fromUtf8("memoryColorButton"));
        sizePolicy1.setHeightForWidth(memoryColorButton->sizePolicy().hasHeightForWidth());
        memoryColorButton->setSizePolicy(sizePolicy1);
        memoryColorButton->setMinimumSize(QSize(23, 23));
        memoryColorButton->setMaximumSize(QSize(23, 23));
        memoryColorButton->setBaseSize(QSize(0, 0));
        memoryColorButton->setFocusPolicy(Qt::NoFocus);
        memoryColorButton->setAutoDefault(false);
        memoryColorButton->setFlat(false);

        gridLayout_2->addWidget(memoryColorButton, 4, 1, 1, 1);

        systemColorButton_2 = new QPushButton(groupBox);
        systemColorButton_2->setObjectName(QString::fromUtf8("systemColorButton_2"));
        sizePolicy1.setHeightForWidth(systemColorButton_2->sizePolicy().hasHeightForWidth());
        systemColorButton_2->setSizePolicy(sizePolicy1);
        systemColorButton_2->setMinimumSize(QSize(23, 23));
        systemColorButton_2->setMaximumSize(QSize(23, 23));
        systemColorButton_2->setBaseSize(QSize(0, 0));
        systemColorButton_2->setFocusPolicy(Qt::NoFocus);
        systemColorButton_2->setAutoDefault(false);
        systemColorButton_2->setFlat(false);

        gridLayout_2->addWidget(systemColorButton_2, 7, 2, 1, 1);

        iomacroLabel_2 = new QLabel(groupBox);
        iomacroLabel_2->setObjectName(QString::fromUtf8("iomacroLabel_2"));

        gridLayout_2->addWidget(iomacroLabel_2, 9, 0, 1, 1);

        labelsColorButton_2 = new QPushButton(groupBox);
        labelsColorButton_2->setObjectName(QString::fromUtf8("labelsColorButton_2"));
        sizePolicy1.setHeightForWidth(labelsColorButton_2->sizePolicy().hasHeightForWidth());
        labelsColorButton_2->setSizePolicy(sizePolicy1);
        labelsColorButton_2->setMinimumSize(QSize(23, 23));
        labelsColorButton_2->setMaximumSize(QSize(23, 23));
        labelsColorButton_2->setBaseSize(QSize(0, 0));
        labelsColorButton_2->setFocusPolicy(Qt::NoFocus);
        labelsColorButton_2->setAutoDefault(false);
        labelsColorButton_2->setFlat(false);

        gridLayout_2->addWidget(labelsColorButton_2, 5, 2, 1, 1);

        registersBoldCheckBox = new QCheckBox(groupBox);
        registersBoldCheckBox->setObjectName(QString::fromUtf8("registersBoldCheckBox"));
        sizePolicy3.setHeightForWidth(registersBoldCheckBox->sizePolicy().hasHeightForWidth());
        registersBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(registersBoldCheckBox, 2, 3, 1, 1);

        quotationBoldCheckBox = new QCheckBox(groupBox);
        quotationBoldCheckBox->setObjectName(QString::fromUtf8("quotationBoldCheckBox"));
        sizePolicy3.setHeightForWidth(quotationBoldCheckBox->sizePolicy().hasHeightForWidth());
        quotationBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(quotationBoldCheckBox, 9, 3, 1, 1);

        iomacroColorButton = new QPushButton(groupBox);
        iomacroColorButton->setObjectName(QString::fromUtf8("iomacroColorButton"));
        sizePolicy1.setHeightForWidth(iomacroColorButton->sizePolicy().hasHeightForWidth());
        iomacroColorButton->setSizePolicy(sizePolicy1);
        iomacroColorButton->setMinimumSize(QSize(23, 23));
        iomacroColorButton->setMaximumSize(QSize(23, 23));
        iomacroColorButton->setBaseSize(QSize(0, 0));
        iomacroColorButton->setFocusPolicy(Qt::NoFocus);
        iomacroColorButton->setAutoDefault(false);
        iomacroColorButton->setFlat(false);

        gridLayout_2->addWidget(iomacroColorButton, 8, 1, 1, 1);

        iomacroItalicCheckBox = new QCheckBox(groupBox);
        iomacroItalicCheckBox->setObjectName(QString::fromUtf8("iomacroItalicCheckBox"));
        sizePolicy3.setHeightForWidth(iomacroItalicCheckBox->sizePolicy().hasHeightForWidth());
        iomacroItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(iomacroItalicCheckBox, 8, 4, 1, 1);

        registersColorButton_2 = new QPushButton(groupBox);
        registersColorButton_2->setObjectName(QString::fromUtf8("registersColorButton_2"));
        sizePolicy1.setHeightForWidth(registersColorButton_2->sizePolicy().hasHeightForWidth());
        registersColorButton_2->setSizePolicy(sizePolicy1);
        registersColorButton_2->setMinimumSize(QSize(23, 23));
        registersColorButton_2->setMaximumSize(QSize(23, 23));
        registersColorButton_2->setBaseSize(QSize(0, 0));
        registersColorButton_2->setFocusPolicy(Qt::NoFocus);
        registersColorButton_2->setAutoDefault(false);
        registersColorButton_2->setFlat(false);

        gridLayout_2->addWidget(registersColorButton_2, 2, 2, 1, 1);

        keywordsBoldCheckBox = new QCheckBox(groupBox);
        keywordsBoldCheckBox->setObjectName(QString::fromUtf8("keywordsBoldCheckBox"));
        sizePolicy3.setHeightForWidth(keywordsBoldCheckBox->sizePolicy().hasHeightForWidth());
        keywordsBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(keywordsBoldCheckBox, 1, 3, 1, 1);

        memoryBoldCheckBox = new QCheckBox(groupBox);
        memoryBoldCheckBox->setObjectName(QString::fromUtf8("memoryBoldCheckBox"));
        sizePolicy3.setHeightForWidth(memoryBoldCheckBox->sizePolicy().hasHeightForWidth());
        memoryBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(memoryBoldCheckBox, 4, 3, 1, 1);

        numbersColorButton = new QPushButton(groupBox);
        numbersColorButton->setObjectName(QString::fromUtf8("numbersColorButton"));
        sizePolicy1.setHeightForWidth(numbersColorButton->sizePolicy().hasHeightForWidth());
        numbersColorButton->setSizePolicy(sizePolicy1);
        numbersColorButton->setMinimumSize(QSize(23, 23));
        numbersColorButton->setMaximumSize(QSize(23, 23));
        numbersColorButton->setBaseSize(QSize(0, 0));
        numbersColorButton->setFocusPolicy(Qt::NoFocus);
        numbersColorButton->setAutoDefault(false);
        numbersColorButton->setFlat(false);

        gridLayout_2->addWidget(numbersColorButton, 3, 1, 1, 1);

        quotationItalicCheckBox = new QCheckBox(groupBox);
        quotationItalicCheckBox->setObjectName(QString::fromUtf8("quotationItalicCheckBox"));
        sizePolicy3.setHeightForWidth(quotationItalicCheckBox->sizePolicy().hasHeightForWidth());
        quotationItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(quotationItalicCheckBox, 9, 4, 1, 1);

        commentsColorButton = new QPushButton(groupBox);
        commentsColorButton->setObjectName(QString::fromUtf8("commentsColorButton"));
        sizePolicy1.setHeightForWidth(commentsColorButton->sizePolicy().hasHeightForWidth());
        commentsColorButton->setSizePolicy(sizePolicy1);
        commentsColorButton->setMinimumSize(QSize(23, 23));
        commentsColorButton->setMaximumSize(QSize(23, 23));
        commentsColorButton->setBaseSize(QSize(0, 0));
        commentsColorButton->setFocusPolicy(Qt::NoFocus);
        commentsColorButton->setAutoDefault(false);
        commentsColorButton->setFlat(false);

        gridLayout_2->addWidget(commentsColorButton, 6, 1, 1, 1);

        labelsItalicCheckBox = new QCheckBox(groupBox);
        labelsItalicCheckBox->setObjectName(QString::fromUtf8("labelsItalicCheckBox"));
        sizePolicy3.setHeightForWidth(labelsItalicCheckBox->sizePolicy().hasHeightForWidth());
        labelsItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(labelsItalicCheckBox, 5, 4, 1, 1);

        memoryLabel = new QLabel(groupBox);
        memoryLabel->setObjectName(QString::fromUtf8("memoryLabel"));

        gridLayout_2->addWidget(memoryLabel, 4, 0, 1, 1);

        iomacroLabel = new QLabel(groupBox);
        iomacroLabel->setObjectName(QString::fromUtf8("iomacroLabel"));

        gridLayout_2->addWidget(iomacroLabel, 8, 0, 1, 1);

        keywordsItalicCheckBox = new QCheckBox(groupBox);
        keywordsItalicCheckBox->setObjectName(QString::fromUtf8("keywordsItalicCheckBox"));
        sizePolicy3.setHeightForWidth(keywordsItalicCheckBox->sizePolicy().hasHeightForWidth());
        keywordsItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(keywordsItalicCheckBox, 1, 4, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_3, 10, 1, 1, 1);

        registersLabel = new QLabel(groupBox);
        registersLabel->setObjectName(QString::fromUtf8("registersLabel"));

        gridLayout_2->addWidget(registersLabel, 2, 0, 1, 1);

        commentsItalicCheckBox = new QCheckBox(groupBox);
        commentsItalicCheckBox->setObjectName(QString::fromUtf8("commentsItalicCheckBox"));
        sizePolicy3.setHeightForWidth(commentsItalicCheckBox->sizePolicy().hasHeightForWidth());
        commentsItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(commentsItalicCheckBox, 6, 4, 1, 1);

        labelsBoldCheckBox = new QCheckBox(groupBox);
        labelsBoldCheckBox->setObjectName(QString::fromUtf8("labelsBoldCheckBox"));
        sizePolicy3.setHeightForWidth(labelsBoldCheckBox->sizePolicy().hasHeightForWidth());
        labelsBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(labelsBoldCheckBox, 5, 3, 1, 1);

        registersItalicCheckBox = new QCheckBox(groupBox);
        registersItalicCheckBox->setObjectName(QString::fromUtf8("registersItalicCheckBox"));
        sizePolicy3.setHeightForWidth(registersItalicCheckBox->sizePolicy().hasHeightForWidth());
        registersItalicCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(registersItalicCheckBox, 2, 4, 1, 1);

        systemBoldCheckBox = new QCheckBox(groupBox);
        systemBoldCheckBox->setObjectName(QString::fromUtf8("systemBoldCheckBox"));
        sizePolicy3.setHeightForWidth(systemBoldCheckBox->sizePolicy().hasHeightForWidth());
        systemBoldCheckBox->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(systemBoldCheckBox, 7, 3, 1, 1);

        memoryColorButton_2 = new QPushButton(groupBox);
        memoryColorButton_2->setObjectName(QString::fromUtf8("memoryColorButton_2"));
        sizePolicy1.setHeightForWidth(memoryColorButton_2->sizePolicy().hasHeightForWidth());
        memoryColorButton_2->setSizePolicy(sizePolicy1);
        memoryColorButton_2->setMinimumSize(QSize(23, 23));
        memoryColorButton_2->setMaximumSize(QSize(23, 23));
        memoryColorButton_2->setBaseSize(QSize(0, 0));
        memoryColorButton_2->setFocusPolicy(Qt::NoFocus);
        memoryColorButton_2->setAutoDefault(false);
        memoryColorButton_2->setFlat(false);

        gridLayout_2->addWidget(memoryColorButton_2, 4, 2, 1, 1);

        registersColorButton = new QPushButton(groupBox);
        registersColorButton->setObjectName(QString::fromUtf8("registersColorButton"));
        sizePolicy1.setHeightForWidth(registersColorButton->sizePolicy().hasHeightForWidth());
        registersColorButton->setSizePolicy(sizePolicy1);
        registersColorButton->setMinimumSize(QSize(23, 23));
        registersColorButton->setMaximumSize(QSize(23, 23));
        registersColorButton->setBaseSize(QSize(0, 0));
        registersColorButton->setFocusPolicy(Qt::NoFocus);
        registersColorButton->setAutoDefault(false);
        registersColorButton->setFlat(false);

        gridLayout_2->addWidget(registersColorButton, 2, 1, 1, 1);

        numbersColorButton_2 = new QPushButton(groupBox);
        numbersColorButton_2->setObjectName(QString::fromUtf8("numbersColorButton_2"));
        sizePolicy1.setHeightForWidth(numbersColorButton_2->sizePolicy().hasHeightForWidth());
        numbersColorButton_2->setSizePolicy(sizePolicy1);
        numbersColorButton_2->setMinimumSize(QSize(23, 23));
        numbersColorButton_2->setMaximumSize(QSize(23, 23));
        numbersColorButton_2->setBaseSize(QSize(0, 0));
        numbersColorButton_2->setFocusPolicy(Qt::NoFocus);
        numbersColorButton_2->setAutoDefault(false);
        numbersColorButton_2->setFlat(false);

        gridLayout_2->addWidget(numbersColorButton_2, 3, 2, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_7, 5, 5, 1, 1);


        gridLayout->addWidget(groupBox, 0, 2, 1, 1);

        groupBox_2 = new QGroupBox(colorsTab);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_5 = new QVBoxLayout(groupBox_2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        fontLabel_2 = new QLabel(groupBox_2);
        fontLabel_2->setObjectName(QString::fromUtf8("fontLabel_2"));

        gridLayout_4->addWidget(fontLabel_2, 4, 0, 1, 1);

        fontColorButton = new QPushButton(groupBox_2);
        fontColorButton->setObjectName(QString::fromUtf8("fontColorButton"));
        sizePolicy1.setHeightForWidth(fontColorButton->sizePolicy().hasHeightForWidth());
        fontColorButton->setSizePolicy(sizePolicy1);
        fontColorButton->setMinimumSize(QSize(23, 23));
        fontColorButton->setMaximumSize(QSize(23, 23));
        fontColorButton->setBaseSize(QSize(0, 0));
        fontColorButton->setFocusPolicy(Qt::NoFocus);
        fontColorButton->setAutoDefault(false);
        fontColorButton->setFlat(false);

        gridLayout_4->addWidget(fontColorButton, 4, 1, 1, 1);

        currentLineCheckBox = new QCheckBox(groupBox_2);
        currentLineCheckBox->setObjectName(QString::fromUtf8("currentLineCheckBox"));

        gridLayout_4->addWidget(currentLineCheckBox, 5, 2, 1, 1);

        debugLineColorButton = new QPushButton(groupBox_2);
        debugLineColorButton->setObjectName(QString::fromUtf8("debugLineColorButton"));
        sizePolicy1.setHeightForWidth(debugLineColorButton->sizePolicy().hasHeightForWidth());
        debugLineColorButton->setSizePolicy(sizePolicy1);
        debugLineColorButton->setMinimumSize(QSize(23, 23));
        debugLineColorButton->setMaximumSize(QSize(23, 23));
        debugLineColorButton->setBaseSize(QSize(0, 0));
        debugLineColorButton->setFocusPolicy(Qt::NoFocus);
        debugLineColorButton->setAutoDefault(false);
        debugLineColorButton->setFlat(false);

        gridLayout_4->addWidget(debugLineColorButton, 6, 1, 1, 1);

        currentLineLabel = new QLabel(groupBox_2);
        currentLineLabel->setObjectName(QString::fromUtf8("currentLineLabel"));

        gridLayout_4->addWidget(currentLineLabel, 5, 0, 1, 1);

        debugLineLabel = new QLabel(groupBox_2);
        debugLineLabel->setObjectName(QString::fromUtf8("debugLineLabel"));

        gridLayout_4->addWidget(debugLineLabel, 6, 0, 1, 1);

        currentLineColorButton = new QPushButton(groupBox_2);
        currentLineColorButton->setObjectName(QString::fromUtf8("currentLineColorButton"));
        sizePolicy1.setHeightForWidth(currentLineColorButton->sizePolicy().hasHeightForWidth());
        currentLineColorButton->setSizePolicy(sizePolicy1);
        currentLineColorButton->setMinimumSize(QSize(23, 23));
        currentLineColorButton->setMaximumSize(QSize(23, 23));
        currentLineColorButton->setBaseSize(QSize(0, 0));
        currentLineColorButton->setFocusPolicy(Qt::NoFocus);
        currentLineColorButton->setAutoDefault(false);
        currentLineColorButton->setFlat(false);

        gridLayout_4->addWidget(currentLineColorButton, 5, 1, 1, 1);

        backgroundLabel = new QLabel(groupBox_2);
        backgroundLabel->setObjectName(QString::fromUtf8("backgroundLabel"));

        gridLayout_4->addWidget(backgroundLabel, 1, 0, 1, 1);

        backgroundColorButton = new QPushButton(groupBox_2);
        backgroundColorButton->setObjectName(QString::fromUtf8("backgroundColorButton"));
        sizePolicy1.setHeightForWidth(backgroundColorButton->sizePolicy().hasHeightForWidth());
        backgroundColorButton->setSizePolicy(sizePolicy1);
        backgroundColorButton->setMinimumSize(QSize(23, 23));
        backgroundColorButton->setMaximumSize(QSize(23, 23));
        backgroundColorButton->setBaseSize(QSize(0, 0));
        backgroundColorButton->setFocusPolicy(Qt::NoFocus);
        backgroundColorButton->setAutoDefault(false);
        backgroundColorButton->setFlat(false);

        gridLayout_4->addWidget(backgroundColorButton, 1, 1, 1, 1);

        lineNumberPanelLabel = new QLabel(groupBox_2);
        lineNumberPanelLabel->setObjectName(QString::fromUtf8("lineNumberPanelLabel"));

        gridLayout_4->addWidget(lineNumberPanelLabel, 2, 0, 1, 1);

        lineNumberPanelColorButton = new QPushButton(groupBox_2);
        lineNumberPanelColorButton->setObjectName(QString::fromUtf8("lineNumberPanelColorButton"));
        sizePolicy1.setHeightForWidth(lineNumberPanelColorButton->sizePolicy().hasHeightForWidth());
        lineNumberPanelColorButton->setSizePolicy(sizePolicy1);
        lineNumberPanelColorButton->setMinimumSize(QSize(23, 23));
        lineNumberPanelColorButton->setMaximumSize(QSize(23, 23));
        lineNumberPanelColorButton->setBaseSize(QSize(0, 0));
        lineNumberPanelColorButton->setFocusPolicy(Qt::NoFocus);
        lineNumberPanelColorButton->setAutoDefault(false);
        lineNumberPanelColorButton->setFlat(false);

        gridLayout_4->addWidget(lineNumberPanelColorButton, 2, 1, 1, 1);

        lineNumberFontLabel = new QLabel(groupBox_2);
        lineNumberFontLabel->setObjectName(QString::fromUtf8("lineNumberFontLabel"));

        gridLayout_4->addWidget(lineNumberFontLabel, 3, 0, 1, 1);

        lineNumberFontColorButton = new QPushButton(groupBox_2);
        lineNumberFontColorButton->setObjectName(QString::fromUtf8("lineNumberFontColorButton"));
        sizePolicy1.setHeightForWidth(lineNumberFontColorButton->sizePolicy().hasHeightForWidth());
        lineNumberFontColorButton->setSizePolicy(sizePolicy1);
        lineNumberFontColorButton->setMinimumSize(QSize(23, 23));
        lineNumberFontColorButton->setMaximumSize(QSize(23, 23));
        lineNumberFontColorButton->setBaseSize(QSize(0, 0));
        lineNumberFontColorButton->setFocusPolicy(Qt::NoFocus);
        lineNumberFontColorButton->setAutoDefault(false);
        lineNumberFontColorButton->setFlat(false);

        gridLayout_4->addWidget(lineNumberFontColorButton, 3, 1, 1, 1);


        verticalLayout_5->addLayout(gridLayout_4);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        verticalLayout_5->addWidget(label_8);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_5->addItem(horizontalSpacer_6);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_4);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 1);

        tabWidget->addTab(colorsTab, QString());
        buildTab = new QWidget();
        buildTab->setObjectName(QString::fromUtf8("buildTab"));
        verticalLayout_6 = new QVBoxLayout(buildTab);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setHorizontalSpacing(12);
        formLayout->setVerticalSpacing(12);
        formLayout->setContentsMargins(-1, -1, -1, 0);
        modeLabel = new QLabel(buildTab);
        modeLabel->setObjectName(QString::fromUtf8("modeLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, modeLabel);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        x86RadioButton = new QRadioButton(buildTab);
        buttonGroup_2 = new QButtonGroup(SettingsWindow);
        buttonGroup_2->setObjectName(QString::fromUtf8("buttonGroup_2"));
        buttonGroup_2->addButton(x86RadioButton);
        x86RadioButton->setObjectName(QString::fromUtf8("x86RadioButton"));
        x86RadioButton->setChecked(true);

        horizontalLayout_3->addWidget(x86RadioButton);

        x64RadioButton = new QRadioButton(buildTab);
        buttonGroup_2->addButton(x64RadioButton);
        x64RadioButton->setObjectName(QString::fromUtf8("x64RadioButton"));
        x64RadioButton->setChecked(false);

        horizontalLayout_3->addWidget(x64RadioButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        formLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout_3);

        assemblerLabel = new QLabel(buildTab);
        assemblerLabel->setObjectName(QString::fromUtf8("assemblerLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, assemblerLabel);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        nasmRadioButton = new QRadioButton(buildTab);
        buttonGroup = new QButtonGroup(SettingsWindow);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(nasmRadioButton);
        nasmRadioButton->setObjectName(QString::fromUtf8("nasmRadioButton"));
        nasmRadioButton->setChecked(true);

        horizontalLayout_6->addWidget(nasmRadioButton);

        gasRadioButton = new QRadioButton(buildTab);
        buttonGroup->addButton(gasRadioButton);
        gasRadioButton->setObjectName(QString::fromUtf8("gasRadioButton"));

        horizontalLayout_6->addWidget(gasRadioButton);

        fasmRadioButton = new QRadioButton(buildTab);
        buttonGroup->addButton(fasmRadioButton);
        fasmRadioButton->setObjectName(QString::fromUtf8("fasmRadioButton"));

        horizontalLayout_6->addWidget(fasmRadioButton);

        masmRadioButton = new QRadioButton(buildTab);
        buttonGroup->addButton(masmRadioButton);
        masmRadioButton->setObjectName(QString::fromUtf8("masmRadioButton"));

        horizontalLayout_6->addWidget(masmRadioButton);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_8);


        formLayout->setLayout(1, QFormLayout::FieldRole, horizontalLayout_6);

        assemblyLabel = new QLabel(buildTab);
        assemblyLabel->setObjectName(QString::fromUtf8("assemblyLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, assemblyLabel);

        assemblyOptionsEdit = new QLineEdit(buildTab);
        assemblyOptionsEdit->setObjectName(QString::fromUtf8("assemblyOptionsEdit"));

        formLayout->setWidget(2, QFormLayout::FieldRole, assemblyOptionsEdit);

        linkingLabel = new QLabel(buildTab);
        linkingLabel->setObjectName(QString::fromUtf8("linkingLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, linkingLabel);

        linkingOptionsEdit = new QLineEdit(buildTab);
        linkingOptionsEdit->setObjectName(QString::fromUtf8("linkingOptionsEdit"));

        formLayout->setWidget(3, QFormLayout::FieldRole, linkingOptionsEdit);

        assemblerPathLabel = new QLabel(buildTab);
        assemblerPathLabel->setObjectName(QString::fromUtf8("assemblerPathLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, assemblerPathLabel);

        assemblerPathEdit = new QLineEdit(buildTab);
        assemblerPathEdit->setObjectName(QString::fromUtf8("assemblerPathEdit"));

        formLayout->setWidget(4, QFormLayout::FieldRole, assemblerPathEdit);

        linkerPathLabel = new QLabel(buildTab);
        linkerPathLabel->setObjectName(QString::fromUtf8("linkerPathLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, linkerPathLabel);

        linkerPathEdit = new QLineEdit(buildTab);
        linkerPathEdit->setObjectName(QString::fromUtf8("linkerPathEdit"));

        formLayout->setWidget(5, QFormLayout::FieldRole, linkerPathEdit);

        objectFileNameLabel = new QLabel(buildTab);
        objectFileNameLabel->setObjectName(QString::fromUtf8("objectFileNameLabel"));

        formLayout->setWidget(6, QFormLayout::LabelRole, objectFileNameLabel);

        objectFileNameEdit = new QLineEdit(buildTab);
        objectFileNameEdit->setObjectName(QString::fromUtf8("objectFileNameEdit"));

        formLayout->setWidget(6, QFormLayout::FieldRole, objectFileNameEdit);

        gdbPathLabel = new QLabel(buildTab);
        gdbPathLabel->setObjectName(QString::fromUtf8("gdbPathLabel"));

        formLayout->setWidget(7, QFormLayout::LabelRole, gdbPathLabel);

        gdbPathEdit = new QLineEdit(buildTab);
        gdbPathEdit->setObjectName(QString::fromUtf8("gdbPathEdit"));

        formLayout->setWidget(7, QFormLayout::FieldRole, gdbPathEdit);

        assemblerWorkingDirectoryLabel = new QLabel(buildTab);
        assemblerWorkingDirectoryLabel->setObjectName(QString::fromUtf8("assemblerWorkingDirectoryLabel"));

        formLayout->setWidget(11, QFormLayout::LabelRole, assemblerWorkingDirectoryLabel);

        runInCurrentDirectoryCheckbox = new QCheckBox(buildTab);
        runInCurrentDirectoryCheckbox->setObjectName(QString::fromUtf8("runInCurrentDirectoryCheckbox"));
        runInCurrentDirectoryCheckbox->setEnabled(true);

        formLayout->setWidget(11, QFormLayout::FieldRole, runInCurrentDirectoryCheckbox);

        disableLinkingLabel = new QLabel(buildTab);
        disableLinkingLabel->setObjectName(QString::fromUtf8("disableLinkingLabel"));

        formLayout->setWidget(12, QFormLayout::LabelRole, disableLinkingLabel);

        disableLinkingCheckbox = new QCheckBox(buildTab);
        disableLinkingCheckbox->setObjectName(QString::fromUtf8("disableLinkingCheckbox"));
        disableLinkingCheckbox->setEnabled(true);

        formLayout->setWidget(12, QFormLayout::FieldRole, disableLinkingCheckbox);

        gdbVerboseLabel = new QLabel(buildTab);
        gdbVerboseLabel->setObjectName(QString::fromUtf8("gdbVerboseLabel"));

        formLayout->setWidget(10, QFormLayout::LabelRole, gdbVerboseLabel);

        sasmVerboseCheckBox = new QCheckBox(buildTab);
        sasmVerboseCheckBox->setObjectName(QString::fromUtf8("sasmVerboseCheckBox"));

        formLayout->setWidget(10, QFormLayout::FieldRole, sasmVerboseCheckBox);

        MiModusLabel = new QLabel(buildTab);
        MiModusLabel->setObjectName(QString::fromUtf8("MiModusLabel"));

        formLayout->setWidget(13, QFormLayout::LabelRole, MiModusLabel);

        MiModusCheckBox = new QCheckBox(buildTab);
        MiModusCheckBox->setObjectName(QString::fromUtf8("MiModusCheckBox"));
        MiModusCheckBox->setChecked(true);

        formLayout->setWidget(13, QFormLayout::FieldRole, MiModusCheckBox);

        gdbDisplayLabel = new QLabel(buildTab);
        gdbDisplayLabel->setObjectName(QString::fromUtf8("gdbDisplayLabel"));

        formLayout->setWidget(14, QFormLayout::LabelRole, gdbDisplayLabel);

        sasmDisplayCheckBox = new QCheckBox(buildTab);
        sasmDisplayCheckBox->setObjectName(QString::fromUtf8("sasmDisplayCheckBox"));

        formLayout->setWidget(14, QFormLayout::FieldRole, sasmDisplayCheckBox);


        verticalLayout_6->addLayout(formLayout);


        infoLabel = new QLabel(buildTab);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
        infoLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        verticalLayout_6->addWidget(infoLabel);

        tabWidget->addTab(buildTab, QString());

        verticalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(SettingsWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

        QWidget::setTabOrder(startWindow, fontSizeSpinBox);
        QWidget::setTabOrder(fontSizeSpinBox, fontComboBox);
        QWidget::setTabOrder(fontComboBox, resetAllButton);
        QWidget::setTabOrder(resetAllButton, language);

        retranslateUi(SettingsWindow);

        tabWidget->setCurrentIndex(2);
        quotationColorButton->setDefault(false);
        systemColorButton->setDefault(false);
        commentsColorButton_2->setDefault(false);
        keywordsColorButton->setDefault(false);
        keywordsColorButton_2->setDefault(false);
        iomacroColorButton_2->setDefault(false);
        labelsColorButton->setDefault(false);
        quotationColorButton_2->setDefault(false);
        memoryColorButton->setDefault(false);
        systemColorButton_2->setDefault(false);
        labelsColorButton_2->setDefault(false);
        iomacroColorButton->setDefault(false);
        registersColorButton_2->setDefault(false);
        numbersColorButton->setDefault(false);
        commentsColorButton->setDefault(false);
        memoryColorButton_2->setDefault(false);
        registersColorButton->setDefault(false);
        numbersColorButton_2->setDefault(false);
        fontColorButton->setDefault(false);
        debugLineColorButton->setDefault(false);
        currentLineColorButton->setDefault(false);
        backgroundColorButton->setDefault(false);
        lineNumberPanelColorButton->setDefault(false);
        lineNumberFontColorButton->setDefault(false);


        QMetaObject::connectSlotsByName(SettingsWindow);
    } // setupUi

    void retranslateUi(QWidget *SettingsWindow)
    {
        SettingsWindow->setWindowTitle(QApplication::translate("SettingsWindow", "Settings", nullptr));
        settingsLabel->setText(QApplication::translate("SettingsWindow", "SASM Options", nullptr));
        commonSettingsBox->setTitle(QApplication::translate("SettingsWindow", "Common", nullptr));
        startWindowLabel->setText(QApplication::translate("SettingsWindow", "On start:", nullptr));
        startWindow->setItemText(0, QApplication::translate("SettingsWindow", "Open get started window", nullptr));
        startWindow->setItemText(1, QApplication::translate("SettingsWindow", "Restore previous session", nullptr));

        languageLabel->setText(QApplication::translate("SettingsWindow", "Language:", nullptr));

        label_4->setText(QApplication::translate("SettingsWindow", "To apply the changes require a restart!", nullptr));
        registersLabel_2->setText(QApplication::translate("SettingsWindow", "Show all registers in debug:", nullptr));
        registersYesRadioButton->setText(QApplication::translate("SettingsWindow", "Yes", nullptr));
        registersNoRadioButton->setText(QApplication::translate("SettingsWindow", "No, show only general purpose", nullptr));
        insertDebugStringLabel->setText(QApplication::translate("SettingsWindow", "Insert debug string:", nullptr));
        insertDebugStringCheckBox->setText(QString());
        codeSettingsBox->setTitle(QApplication::translate("SettingsWindow", "Code editor", nullptr));
        fontLabel->setText(QApplication::translate("SettingsWindow", "Font:", nullptr));
        fontSizeLabel->setText(QApplication::translate("SettingsWindow", "Size:", nullptr));
        label->setText(QApplication::translate("SettingsWindow", "To apply the changes require a restart!", nullptr));
        label_2->setText(QApplication::translate("SettingsWindow", "Default code editor text:", nullptr));
        resetAllButton->setText(QApplication::translate("SettingsWindow", "Reset all (need a restart)...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(commonTab), QApplication::translate("SettingsWindow", "Common", nullptr));
        groupBox->setTitle(QApplication::translate("SettingsWindow", "Syntax highlighting", nullptr));
        iomacroBoldCheckBox->setText(QString());
        systemLabel->setText(QApplication::translate("SettingsWindow", "System:", nullptr));
        quotationColorButton->setText(QString());
        systemColorButton->setText(QString());
        numbersItalicCheckBox->setText(QString());
        label_6->setText(QApplication::translate("SettingsWindow", "Bold:", nullptr));
        numbersBoldCheckBox->setText(QString());
        label_7->setText(QApplication::translate("SettingsWindow", "Italic:", nullptr));
        label_3->setText(QApplication::translate("SettingsWindow", "Foreground:", nullptr));
        systemItalicCheckBox->setText(QString());
        commentsColorButton_2->setText(QString());
        keywordsColorButton->setText(QString());
        commentsBoldCheckBox->setText(QString());
        keywordsColorButton_2->setText(QString());
        iomacroColorButton_2->setText(QString());
        memoryItalicCheckBox->setText(QString());
        keywordsLabel->setText(QApplication::translate("SettingsWindow", "Keywords:", nullptr));
        label_5->setText(QApplication::translate("SettingsWindow", "Background:", nullptr));
        labelsColorButton->setText(QString());
        numbersLabel->setText(QApplication::translate("SettingsWindow", "Numbers:", nullptr));
        quotationColorButton_2->setText(QString());
        labelsLabel->setText(QApplication::translate("SettingsWindow", "Labels:", nullptr));
        commentsLabel->setText(QApplication::translate("SettingsWindow", "Comments:", nullptr));
        memoryColorButton->setText(QString());
        systemColorButton_2->setText(QString());
        iomacroLabel_2->setText(QApplication::translate("SettingsWindow", "Quotation:", nullptr));
        labelsColorButton_2->setText(QString());
        registersBoldCheckBox->setText(QString());
        quotationBoldCheckBox->setText(QString());
        iomacroColorButton->setText(QString());
        iomacroItalicCheckBox->setText(QString());
        registersColorButton_2->setText(QString());
        keywordsBoldCheckBox->setText(QString());
        memoryBoldCheckBox->setText(QString());
        numbersColorButton->setText(QString());
        quotationItalicCheckBox->setText(QString());
        commentsColorButton->setText(QString());
        labelsItalicCheckBox->setText(QString());
        memoryLabel->setText(QApplication::translate("SettingsWindow", "Memory:", nullptr));
        iomacroLabel->setText(QApplication::translate("SettingsWindow", "I/O macro:", nullptr));
        keywordsItalicCheckBox->setText(QString());
        registersLabel->setText(QApplication::translate("SettingsWindow", "Registers:", nullptr));
        commentsItalicCheckBox->setText(QString());
        labelsBoldCheckBox->setText(QString());
        registersItalicCheckBox->setText(QString());
        systemBoldCheckBox->setText(QString());
        memoryColorButton_2->setText(QString());
        registersColorButton->setText(QString());
        numbersColorButton_2->setText(QString());
        groupBox_2->setTitle(QApplication::translate("SettingsWindow", "Common", nullptr));
        fontLabel_2->setText(QApplication::translate("SettingsWindow", "Font:", nullptr));
        fontColorButton->setText(QString());
        currentLineCheckBox->setText(QApplication::translate("SettingsWindow", "Enable highlighting", nullptr));
        debugLineColorButton->setText(QString());
        currentLineLabel->setText(QApplication::translate("SettingsWindow", "Current line:", nullptr));
        debugLineLabel->setText(QApplication::translate("SettingsWindow", "Debugging line:", nullptr));
        currentLineColorButton->setText(QString());
        backgroundLabel->setText(QApplication::translate("SettingsWindow", "Background:", nullptr));
        backgroundColorButton->setText(QString());
        lineNumberPanelLabel->setText(QApplication::translate("SettingsWindow", "Line number panel:", nullptr));
        lineNumberPanelColorButton->setText(QString());
        lineNumberFontLabel->setText(QApplication::translate("SettingsWindow", "Line number font:", nullptr));
        lineNumberFontColorButton->setText(QString());
        label_8->setText(QApplication::translate("SettingsWindow", "To apply the changes require a restart!", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(colorsTab), QApplication::translate("SettingsWindow", "Colors", nullptr));
        modeLabel->setText(QApplication::translate("SettingsWindow", "Mode:", nullptr));
        x86RadioButton->setText(QApplication::translate("SettingsWindow", "x86", nullptr));
        x64RadioButton->setText(QApplication::translate("SettingsWindow", "x64", nullptr));
        assemblerLabel->setText(QApplication::translate("SettingsWindow", "Assembler:", nullptr));
        nasmRadioButton->setText(QApplication::translate("SettingsWindow", "NASM", nullptr));
        gasRadioButton->setText(QApplication::translate("SettingsWindow", "GAS", nullptr));
        fasmRadioButton->setText(QApplication::translate("SettingsWindow", "FASM", nullptr));
        masmRadioButton->setText(QApplication::translate("SettingsWindow", "MASM", nullptr));
        assemblyLabel->setText(QApplication::translate("SettingsWindow", "Assembly options:", nullptr));
        linkingLabel->setText(QApplication::translate("SettingsWindow", "Linking options:", nullptr));
        assemblerPathLabel->setText(QApplication::translate("SettingsWindow", "Assembler path:", nullptr));
        linkerPathLabel->setText(QApplication::translate("SettingsWindow", "Linker path:", nullptr));
        objectFileNameLabel->setText(QApplication::translate("SettingsWindow", "Object file name:", nullptr));
        gdbPathLabel->setText(QApplication::translate("SettingsWindow", "GDB path (Unix only):", nullptr));
        assemblerWorkingDirectoryLabel->setText(QApplication::translate("SettingsWindow", "Build in current directory:", nullptr));
        runInCurrentDirectoryCheckbox->setText(QString());
        disableLinkingLabel->setText(QApplication::translate("SettingsWindow", "Disable linking:", nullptr));
        disableLinkingCheckbox->setText(QString());
        gdbVerboseLabel->setText(QApplication::translate("SettingsWindow", "SASM verbose mode:", nullptr));
        sasmVerboseCheckBox->setText(QString());
        MiModusLabel->setText(QApplication::translate("SettingsWindow", "MI-Mode:", nullptr));
        MiModusCheckBox->setText(QString());
        gdbDisplayLabel->setText(QApplication::translate("SettingsWindow", "SASM display:", nullptr));
        sasmDisplayCheckBox->setText(QString());
        infoLabel->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(buildTab), QApplication::translate("SettingsWindow", "Build", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsWindow: public Ui_SettingsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
