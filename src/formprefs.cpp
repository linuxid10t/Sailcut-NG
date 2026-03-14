/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "formprefs.h"
#include "app.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

/**
 * Constructs the preferences dialog.
 */
CFormPrefs::CFormPrefs(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));

    CSailApp *app = qobject_cast<CSailApp*>(qApp);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Units group box
    QGroupBox *grpUnits = new QGroupBox(tr("Units"), this);
    QVBoxLayout *unitsLayout = new QVBoxLayout(grpUnits);

    radioImperial = new QRadioButton(tr("Imperial (inches)"), this);
    radioMetric = new QRadioButton(tr("Metric (millimeters)"), this);

    if (app->useInches())
        radioImperial->setChecked(true);
    else
        radioMetric->setChecked(true);

    unitsLayout->addWidget(radioImperial);
    unitsLayout->addWidget(radioMetric);

    mainLayout->addWidget(grpUnits);

    // OK/Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

/**
 * Saves the preferences when the user clicks OK.
 */
void CFormPrefs::accept()
{
    CSailApp *app = qobject_cast<CSailApp*>(qApp);
    app->setUseInches(radioImperial->isChecked());
    QDialog::accept();
}
