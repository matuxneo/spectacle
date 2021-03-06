/*
 *  Copyright (C) 2015 Boudhayan Gupta <bgupta@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "SaveOptionsPage.h"

#include "SpectacleConfig.h"

#include <KIOWidgets/KUrlRequester>
#include <KLocalizedString>

#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QComboBox>
#include <QImageWriter>
#include <QCheckBox>

SaveOptionsPage::SaveOptionsPage(QWidget *parent) :
    SettingsPage(parent)
{
    QFormLayout *mainLayout = new QFormLayout;
    setLayout(mainLayout);

    // Save location
    mUrlRequester = new KUrlRequester;
    mUrlRequester->setMode(KFile::Directory);
    connect(mUrlRequester, &KUrlRequester::textChanged, this, &SaveOptionsPage::markDirty);
    mainLayout->addRow(i18n("Save Location:"), mUrlRequester);

    // copy file location to clipboard after saving
    mCopyPathToClipboard = new QCheckBox(i18n("Copy file location to clipboard after saving"), this);
    connect(mCopyPathToClipboard, &QCheckBox::toggled, this, &SaveOptionsPage::markDirty);
    mainLayout->addRow(QString(), mCopyPathToClipboard);


    mainLayout->addItem(new QSpacerItem(0, 18, QSizePolicy::Fixed, QSizePolicy::Fixed));


    // filename chooser and instructional text
    QVBoxLayout *saveNameLayout = new QVBoxLayout;

    // filename chooser text field
    QHBoxLayout *saveFieldLayout = new QHBoxLayout;
    mSaveNameFormat = new QLineEdit;
    connect(mSaveNameFormat, &QLineEdit::textEdited, this, &SaveOptionsPage::markDirty);
    connect(mSaveNameFormat, &QLineEdit::textEdited, [&](const QString &newText) {
        QString fmt;
        Q_FOREACH(auto item, QImageWriter::supportedImageFormats()) {
            fmt = QString::fromLocal8Bit(item);
            if (newText.endsWith(QLatin1Char('.') + fmt, Qt::CaseInsensitive)) {
                QString txtCopy = newText;
                txtCopy.chop(fmt.length() + 1);
                mSaveNameFormat->setText(txtCopy);
                mSaveImageFormat->setCurrentIndex(mSaveImageFormat->findText(fmt.toUpper()));
            }
        }
    });
    mSaveNameFormat->setPlaceholderText(QStringLiteral("%d"));
    saveFieldLayout->addWidget(mSaveNameFormat);

    mSaveImageFormat = new QComboBox;
    mSaveImageFormat->addItems([&](){
        QStringList items;
        Q_FOREACH(auto fmt, QImageWriter::supportedImageFormats()) {
            items.append(QString::fromLocal8Bit(fmt).toUpper());
        }
        return items;
    }());
    connect(mSaveImageFormat, &QComboBox::currentTextChanged, this, &SaveOptionsPage::markDirty);
    saveFieldLayout->addWidget(mSaveImageFormat);
    saveNameLayout->addLayout(saveFieldLayout);

    // now the save filename format layout
    const QString helpText = i18nc("%1 is the default filename of a screenshot",
        "<p>You can use the following placeholders in the filename, which will be replaced "
        "with actual text when the file is saved:</p>"

        "<blockquote>"
            "<b>%Y</b>: Year (4 digit)<br />"
            "<b>%y</b>: Year (2 digit)<br />"
            "<b>%M</b>: Month<br />"
            "<b>%D</b>: Day<br />"
            "<b>%H</b>: Hour<br />"
            "<b>%m</b>: Minute<br />"
            "<b>%S</b>: Second<br />"
            "<b>%T</b>: Window title<br />"
            "<b>%d</b>: Sequential numbering<br />"
            "<b>%Nd</b>: Sequential numbering, padded out to N digits"
        "</blockquote>"

        "<p>To save to a sub-folder, use slashes, e.g.:</p>"

        "<blockquote>"
            "<b>%Y</b>/<b>%M</b>/%1"
        "</blockquote>",
        SpectacleConfig::instance()->defaultFilename() + SpectacleConfig::instance()->defaultTimestampTemplate()
    );

    QLabel *fmtHelpText = new QLabel(helpText, this);
    fmtHelpText->setWordWrap(true);
    fmtHelpText->setTextFormat(Qt::RichText);
    fmtHelpText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    saveNameLayout->addWidget(fmtHelpText);
    mainLayout->addRow(i18n("Filename:"), saveNameLayout);

    // read in the data
    resetChanges();
}

void SaveOptionsPage::markDirty()
{
    mChangesMade = true;
}

void SaveOptionsPage::saveChanges()
{
    // bring up the configuration reader

    SpectacleConfig *cfgManager = SpectacleConfig::instance();

    // save the data

    cfgManager->setDefaultSaveLocation(mUrlRequester->url().toDisplayString(QUrl::PreferLocalFile));
    cfgManager->setAutoSaveFilenameFormat(mSaveNameFormat->text());
    cfgManager->setSaveImageFormat(mSaveImageFormat->currentText().toLower());
    cfgManager->setCopySaveLocationToClipboard(mCopyPathToClipboard->checkState() == Qt::Checked);

    // done

    mChangesMade = false;
}

void SaveOptionsPage::resetChanges()
{
    // bring up the configuration reader

    SpectacleConfig *cfgManager = SpectacleConfig::instance();

    // read in the data

    mSaveNameFormat->setText(cfgManager->autoSaveFilenameFormat());
    mUrlRequester->setUrl(QUrl::fromUserInput(cfgManager->defaultSaveLocation()));
    mCopyPathToClipboard->setChecked(cfgManager->copySaveLocationToClipboard());

    // read in the save image format and calculate its index

    {
        int index = mSaveImageFormat->findText(cfgManager->saveImageFormat().toUpper());
        if (index >= 0) {
            mSaveImageFormat->setCurrentIndex(index);
        }
    }

    // done

    mChangesMade = false;
}
