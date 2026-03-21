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

#include <cmath>

#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>

#include "formhulldef.h"
#include "app.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** The constructor.
 *
 * @param parent the parent window
 * @param hullptr pointer to the CHullDef
 */
CFormHullDef::CFormHullDef( QWidget* parent, CHullDef * hullptr )
        : QDialog(parent)
{
    setupUi(this);
    setModal(true);

    /* we store the pointer to the CHullDef so we can update it when
       the user clicks OK */
    hulldef = hullptr;

    // Get unit preference
    CSailApp *app = qobject_cast<CSailApp*>(qApp);
    m_useInches = app->useInches();

    // Helper lambda for displaying length values
    auto displayLength = [this](double mmValue) -> QString {
        if (m_useInches)
            return QString::number(mmValue / INCH_TO_MM, 'f', 2);
        else
            return QString::number(mmValue);
    };

    // Update the unit label at the top of the dialog
    if (m_useInches)
        label->setText(tr("Dimensions are in inches (or ft'in\") and angles in degrees measured from horizontal"));
    else
        label->setText(tr("Dimensions are in millimeters and angles in degrees measured from horizontal"));

    txt_HullID->setText(QString::fromStdString(hulldef->hullID));
    /// deck parameters
    txt_BLWL->setText( displayLength(hulldef->BLWL) );
    txt_DfwdHeight->setText( displayLength(hulldef->DfwdHeight) );
    txt_DaftHeight->setText( displayLength(hulldef->DaftHeight) );
    txt_BBW->setText( displayLength(hulldef->BBW) );
    txt_BaftW->setText( displayLength(hulldef->BaftW) );
    spinBox_BSlopeA->setValue(hulldef->BSlopeA);
    spinBox_BBWPos->setValue(hulldef->BBWPos);

    spinBox_StemA->setValue(hulldef->StemA);
    spinBox_TransomA->setValue(hulldef->TransomA);

    spinBox_BfwdShape->setValue(hulldef->BfwdShape);
    spinBox_BaftShape->setValue(hulldef->BaftShape);

    /// bottom parameters
    txt_BfwdHeight->setText( displayLength(hulldef->BfwdHeight) );
    txt_BaftHeight->setText( displayLength(hulldef->BaftHeight) );
    spinBox_BSlopeA->setValue(hulldef->BSlopeA);
    spinBox_BDeadriseA->setValue(hulldef->BDeadriseA);
    spinBox_BSweepA->setValue(hulldef->BSweepA);

    /// planking parameters
    spinBox_NBPlank->setValue(hulldef->NBPlank);
    spinBox_TopPlankA->setValue(hulldef->TopPlankA);
    spinBox_LowPlankA->setValue(hulldef->LowPlankA);

    checkBox_AutoPlank->setChecked(hulldef->AutoPlank);

    /// signal and slots connections
    connect( btnCheck, SIGNAL( clicked() ), this, SLOT( slotCheck() ) );
    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // resize
    resize(minimumSizeHint());
}


// Qt overrides

/** Saves the parameters entered by the user in the CHullDef.
 *  slot connected to OK button
 */
void CFormHullDef::accept()
{
    // return data if everything is OK. /////////////////////////
    if (check() == true)
        QDialog::accept();
}

/** slot for dealing with automatic planking
 */
void CFormHullDef::slotAutoPlank()
{
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;

    check();
}

/** slot for dealing with check button
 */
void CFormHullDef::slotCheck()
{
    check();
}


/** Check all dimensions entered in order
 *  to make sure that the hull is possible and reasonable
 */
bool CFormHullDef::check()
{
    ///  return true IF everything is OK
    long L1=1;
    // real A1=0, A2=0;
    bool flag = true;
    QString txt;

    ///  create four palettes
    QPalette palStd, palHi, palLo, palRel;
    palStd = txt_HullID->palette();
    palLo = palHi = palRel = palStd;
    palLo.setColor( QPalette::Text, Qt::magenta); // too low value
    palHi.setColor( QPalette::Text, Qt::red );    // too high value
    palRel.setColor( QPalette::Text, Qt::blue );  // related value to be checked

    // Helper lambda for displaying length values (converts from mm to display units)
    auto displayLength = [this](double mmValue) -> QString {
        if (m_useInches)
            return QString::number(mmValue / INCH_TO_MM, 'f', 2);
        else
            return QString::number(mmValue);
    };

    // Helper lambda for reading length values (converts from display units to mm)
    // Supports fractional inch input like "25 1/2" for 25.5 inches
    auto readLength = [this](const QLineEdit *edit) -> double {
        QString text = edit->text().simplified();
        double value = 0.0;

        if (m_useInches)
        {
            // Helper to parse a number that might include a fraction
            auto parseWithFraction = [](const QString& str) -> double {
                double val = 0.0;
                QStringList parts = str.split(' ', Qt::SkipEmptyParts);
                if (parts.size() == 2)
                {
                    // Whole number + fraction
                    val = parts[0].toDouble();
                    QStringList fractionParts = parts[1].split('/');
                    if (fractionParts.size() == 2)
                    {
                        double numerator = fractionParts[0].toDouble();
                        double denominator = fractionParts[1].toDouble();
                        if (denominator != 0)
                            val += numerator / denominator;
                    }
                }
                else if (parts.size() == 1)
                {
                    if (str.contains('/'))
                    {
                        QStringList fractionParts = str.split('/');
                        if (fractionParts.size() == 2)
                        {
                            double numerator = fractionParts[0].toDouble();
                            double denominator = fractionParts[1].toDouble();
                            if (denominator != 0)
                                val = numerator / denominator;
                        }
                    }
                    else
                    {
                        val = str.toDouble();
                    }
                }
                return val;
            };

            // Check for feet'inches format (e.g., "5'3\"" or "5'3 1/2")
            if (text.contains('\''))
            {
                QStringList feetInches = text.split('\'');
                if (feetInches.size() >= 1)
                {
                    // Feet part
                    value = feetInches[0].toDouble() * 12.0;
                    // Inches part (if present)
                    if (feetInches.size() >= 2)
                    {
                        // Remove trailing " if present
                        QString inchesStr = feetInches[1];
                        if (inchesStr.endsWith('"'))
                            inchesStr.chop(1);
                        value += parseWithFraction(inchesStr);
                    }
                }
            }
            else
            {
                // Just inches (possibly with fraction)
                value = parseWithFraction(text);
            }
            return value * INCH_TO_MM;
        }
        return text.toDouble();
    };

    /// check hull ID
    txt = txt_HullID->text();
    txt = txt.simplified();
    if (txt.length() > 40)
    {
        txt.truncate(40);
        flag = false;
        txt_HullID->setPalette(palHi);
        txt_HullID->setText(QString(txt));
    }
    else
    {
        txt_HullID->setPalette(palStd);
        txt_HullID->setText(QString(txt));
    }
    hulldef->hullID = txt.toStdString();

    /// check bottom data
    hulldef->BLWL = readLength(txt_BLWL); // length waterline

    if (hulldef->BLWL < 100)
    {
        flag = false;
        txt_BLWL->setPalette(palLo);
        hulldef->BLWL = 100;
    }
    else if (hulldef->BLWL > 100000)
    {
        flag = false;
        txt_BLWL->setPalette(palHi);
        hulldef->BLWL = 100000;
    }
    else
    {
        txt_BLWL->setPalette(palStd);
    }
    txt_BLWL->setText(displayLength(hulldef->BLWL));
    L1 = (long)(hulldef->BLWL);

    // lower chine height
    hulldef->BfwdHeight = readLength(txt_BfwdHeight);
    if (hulldef->BfwdHeight > L1/5)
    {
        flag = false;
        txt_BfwdHeight->setPalette(palHi);
        hulldef->BfwdHeight = floor(L1 / 5);
    }
    else
    {
        txt_BfwdHeight->setPalette(palStd);
    }
    txt_BfwdHeight->setText(displayLength(hulldef->BfwdHeight));

    hulldef->BaftHeight = readLength(txt_BaftHeight);
    if (hulldef->BaftHeight > L1 / 5)
    {
        flag = false;
        txt_BaftHeight->setPalette(palHi);
        hulldef->BaftHeight = floor(L1 / 5);
    }
    else
    {
        txt_BaftHeight->setPalette(palStd);
    }
    txt_BaftHeight->setText(displayLength(hulldef->BaftHeight));

    // bottom width
    hulldef->BBW = readLength(txt_BBW);
    if (hulldef->BBW < L1/20)
    {
        flag = false;
        txt_BBW->setPalette(palLo);
        hulldef->BBW = ceil(L1/20);
    }
    else if (hulldef->BBW > L1/2)
    {
        flag = false;
        txt_BBW->setPalette(palHi);
        hulldef->BBW = floor(L1/2);
    }
    else
    {
        txt_BBW->setPalette(palStd);
    }
    txt_BBW->setText(displayLength(hulldef->BBW));

    hulldef->BaftW = readLength(txt_BaftW);
    if (hulldef->BaftW < 0)
    {
        flag = false;
        txt_BaftW->setPalette(palLo);
        hulldef->BaftW = 0;
    }
    else if (hulldef->BaftW > hulldef->BBW)
    {
        flag = false;
        txt_BaftW->setPalette(palHi);
        hulldef->BaftW = floor(hulldef->BBW);
    }
    else
    {
        txt_BaftW->setPalette(palStd);
    }
    txt_BaftW->setText(displayLength(hulldef->BaftW));

    hulldef->BBWPos = spinBox_BBWPos->value();

    // bottom shape
    hulldef->BfwdShape = spinBox_BfwdShape->value();
    hulldef->BaftShape = spinBox_BaftShape->value();

    hulldef->BSlopeA = spinBox_BSlopeA->value();
    hulldef->BDeadriseA = spinBox_BDeadriseA->value();
    hulldef->BSweepA = spinBox_BSweepA->value();

    hulldef->StemA = spinBox_StemA->value();
    hulldef->TransomA = spinBox_TransomA->value();

    /// check deck data
    hulldef->DfwdHeight = readLength(txt_DfwdHeight);
    if (hulldef->DfwdHeight < (hulldef->BfwdHeight + L1 / 20) )
    {
        flag = false;
        txt_DfwdHeight->setPalette(palLo);
        hulldef->DfwdHeight = ceil( hulldef->BfwdHeight + L1 / 20 );
    }
    else if (hulldef->DfwdHeight > ( L1 / 4 ) )
    {
        flag = false;
        txt_DfwdHeight->setPalette(palHi);
        hulldef->DfwdHeight = floor( L1 / 4 );
    }
    else
    {
        txt_DfwdHeight->setPalette(palStd);
    }
    txt_DfwdHeight->setText(displayLength(hulldef->DfwdHeight));

    hulldef->DaftHeight = readLength(txt_DaftHeight);
    if (hulldef->DaftHeight < (hulldef->BaftHeight + L1 / 20) )
    {
        flag = false;
        txt_DaftHeight->setPalette(palLo);
        hulldef->DaftHeight = ceil( hulldef->BaftHeight + L1 / 20);
    }
    else if (hulldef->DaftHeight > ( L1 / 4 ) )
    {
        flag = false;
        txt_DaftHeight->setPalette(palHi);
        hulldef->DaftHeight = floor( L1 / 4 );
    }
    else
    {
        txt_DaftHeight->setPalette(palStd);
    }
    txt_DaftHeight->setText(displayLength(hulldef->DaftHeight));

    /// planking parameters
    hulldef->NBPlank = spinBox_NBPlank->value();
    hulldef->TopPlankA = spinBox_TopPlankA->value();
    hulldef->LowPlankA = spinBox_LowPlankA->value();

    /// check automatic planking
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;
    else
        hulldef->AutoPlank = false;

    ///  return true IF everything is OK
    return flag;
}
