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

#include "GeneralOptionsPage.h"

#include "SpectacleConfig.h"

#include <KLocalizedString>
#include <KTitleWidget>

#include <QButtonGroup>
#include <QCheckBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QSpacerItem>

GeneralOptionsPage::GeneralOptionsPage(QWidget *parent) :
    SettingsPage(parent)
{
    QFormLayout *mainLayout = new QFormLayout(this);
    setLayout(mainLayout);

    // Rectangular Region settings
    KTitleWidget *titleWidget = new KTitleWidget(this);
    titleWidget->setText(i18n("Rectangular Region:"));
    titleWidget->setLevel(2);
    mainLayout->addRow(titleWidget);

    // use light background
    mUseLightBackground = new QCheckBox(i18n("Use light background"), this);
    connect(mUseLightBackground, &QCheckBox::toggled, this, &GeneralOptionsPage::markDirty);
    mainLayout->addRow(i18n("General:"), mUseLightBackground);

    // show magnifier
    mShowMagnifier = new QCheckBox(i18n("Show magnifier"), this);
    connect(mShowMagnifier, &QCheckBox::toggled, this, &GeneralOptionsPage::markDirty);
    mainLayout->addRow(QString(), mShowMagnifier);

    mainLayout->addItem(new QSpacerItem(0, 18, QSizePolicy::Fixed, QSizePolicy::Fixed));

    // remember Rectangular Region box
    QButtonGroup* rememberGroup = new QButtonGroup(this);
    rememberGroup->setExclusive(true);
    QRadioButton* neverButton = new QRadioButton(i18n("Never"), this);
    mRememberAlways = new QRadioButton(i18n("Always"), this);
    mRememberUntilClosed = new QRadioButton(i18n("Until Spectacle is closed"), this);
    rememberGroup->addButton(neverButton);
    rememberGroup->addButton(mRememberAlways);
    rememberGroup->addButton(mRememberUntilClosed);
    neverButton->setChecked(true);
    connect(rememberGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled), this, &GeneralOptionsPage::markDirty);
    mainLayout->addRow(i18n("Remember selected area:"), neverButton);
    mainLayout->addRow(QString(), mRememberAlways);
    mainLayout->addRow(QString(), mRememberUntilClosed );

    // read in the data
    resetChanges();
}

void GeneralOptionsPage::markDirty()
{
    mChangesMade = true;
}

void GeneralOptionsPage::saveChanges()
{
    SpectacleConfig *cfgManager = SpectacleConfig::instance();

    cfgManager->setUseLightRegionMaskColour(mUseLightBackground->checkState() == Qt::Checked);
    cfgManager->setRememberLastRectangularRegion(mRememberUntilClosed->isChecked() || mRememberAlways->isChecked());
    cfgManager->setAlwaysRememberRegion (mRememberAlways->isChecked());
    cfgManager->setShowMagnifierChecked(mShowMagnifier->checkState() == Qt::Checked);

    mChangesMade = false;
}

void GeneralOptionsPage::resetChanges()
{
    SpectacleConfig *cfgManager = SpectacleConfig::instance();

    mUseLightBackground->setChecked(cfgManager->useLightRegionMaskColour());
    mRememberUntilClosed->setChecked(cfgManager->rememberLastRectangularRegion());
    mRememberAlways->setChecked(cfgManager->alwaysRememberRegion());
    mShowMagnifier->setChecked(cfgManager->showMagnifierChecked());

    mChangesMade = false;
}
