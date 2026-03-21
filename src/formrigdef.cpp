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
#include <iostream>

#include <QLabel>
#include <QRadioButton>

#include "formrigdef.h"
#include "sailcpp/rigworker.h"
#include "app.h"


/** The constructor.
 *
 * @param parent the parent window
 * @param rigptr pointer to the CRigDef
 */
CFormRigDef::CFormRigDef( QWidget* parent, CRigDef * rigptr )
        : QDialog(parent)
{
    setupUi(this);
    setModal(true);

    /* we store the pointer to the CRigDef so we can update it when
       the user clicks OK */
    rigdef = rigptr;

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
        lbl_TopFrame->setText(tr("All dimensions are in inches (or ft'in\") and angles in degrees"));
    else
        lbl_TopFrame->setText(tr("All dimensions are in millimeter and angles in degrees"));

    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( btnCheck, SIGNAL( clicked() ), this, SLOT( slotChanged() ) );
    connect( txt_foreI, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_foreJ, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_CSH, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_CSB, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_LSB, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_MH, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_MRnd, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_MRkM, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_MC, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_MW, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( spinBox_MRndPos, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );
    connect( spinBox_SPNB, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );
    connect( txt_HAD, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );
    connect( txt_BAD, SIGNAL( textEdited(QString) ), this, SLOT( slotChanged() ) );

    txt_RigID->setText(QString::fromStdString(rigdef->rigID) );
    txt_foreI->setText( displayLength(rigdef->foreI) );
    txt_foreJ->setText( displayLength(rigdef->foreJ) );
    txt_MH->setText( displayLength(rigdef->MHeight) );
    txt_MC->setText( displayLength(rigdef->MCord) );
    txt_MW->setText( displayLength(rigdef->MWidth) );
    txt_MRnd->setText( displayLength(rigdef->MRnd) );
    txt_MRkM->setText( displayLength(rigdef->MRakeM) );
    spinBox_MRndPos->setValue(rigdef->MRndPos);

    txt_CSH->setText( displayLength(rigdef->CSH) );
    txt_CSB->setText( displayLength(rigdef->CSB) );
    txt_LSB->setText( displayLength(rigdef->LSB) );

    txt_SPH1->setText( displayLength(rigdef->SPH[1]) );
    txt_SPH2->setText( displayLength(rigdef->SPH[2]) );
    txt_SPH3->setText( displayLength(rigdef->SPH[3]) );

    txt_SPW1->setText( displayLength(rigdef->SPW[1]) );
    txt_SPW2->setText( displayLength(rigdef->SPW[2]) );
    txt_SPW3->setText( displayLength(rigdef->SPW[3]) );

    // display
    lbl_MRkD->setText(QString::number(rigdef->MRakeD) );

    spinBox_SPNB->setValue(rigdef->SPNB);

    // variable changes triggering a check
    txt_HAD->setText( displayLength(rigdef->HAD) );
    txt_BAD->setText( displayLength(rigdef->BAD) );

    // resize
    resize(minimumSizeHint());
}


/** Save the parameters entered by the user in the CRigDef.
 *  slot connected to OK button
 */
void CFormRigDef::accept()
{
    // return data if everything is OK
    if (check() ==true)
        QDialog::accept();
}


/** Sets the strings of the subwidgets using the current
 *  language.
 */
void CFormRigDef::languageChange()
{   /* FIXME use proper labels
    lbl_RigID->setText( tr("Rig identification") );
    lbl_foreJ->setText( tr("Fore triangle base") );
    lbl_foreI->setText( tr("Fore triangle height") );
    lbl_MastWidth->setText( tr("Mast width") );
    lbl_MastCord->setText( tr("Mast cord") );
    lbl_MastRakeAngle->setText( tr("Mast rake angle") );
    lbl_MastRake->setText( tr("Mast rake") );
    lbl_MastHeight->setText( tr("Mast height") );
    lbl_MastRound->setText( tr("Mast round") );
    lbl_MastRoundPos->setText( tr("Mast round position") );

    lbl_CapShroudHeight->setText( tr("Cap shroud height") );
    lbl_CapShroudBase->setText( tr("Cap shroud base width") );
    lbl_LowerShroudBase->setText( tr("Lower shroud base width") );
    lbl_SpreaderHeight1->setText( tr("Spreader height") );
    lbl_SpreaderHeight2->setText( tr("Spreader height") );
    lbl_SpreaderHeight3->setText( tr("Spreader height") );
    lbl_SpreaderWidth1->setText( tr("Spreader width") );
    lbl_SpreaderWidth2->setText( tr("Spreader width") );
    lbl_SpreaderWidth3->setText( tr("Spreader width") );
    label_SpreaderNumber->setText( tr("Number of spreaders") );

    lbl_BAD->setText( tr("Mainsail tack height") );
    lbl_HAD->setText( tr("Mainsail head height") );
    lbl_Luff_Round->setText( tr("Luff round") );
    lbl_Luff->setText( tr("Luff length") );

    */
}


/** Called when one of the values changes.
 *  it triggers a check of all data
 */
void CFormRigDef::slotChanged()
{
    /*
    if ( active == false )
        return;
    */

    check();

}


/** Check all dimensions entered in order to
 *  make sure that the rig is possible and reasonable.
 *  This will also trigger ancillary data computation
 *  for mainsail luff parameters.
 */
bool CFormRigDef::check()
{
    long I = 1, J = 1, L1 = 1, L2 = 1;
    bool flag = true;

    unsigned int j, n;

    QString txt;
    ///  create four palettes
    QPalette palStd, palHi, palLo, palRel;
    palStd = txt_RigID->palette();
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

    //  start collecting data
    ///  check the rig ID text
    txt = txt_RigID->text();
    txt = txt.simplified();
    if (txt.length() > 40)
    {
        txt.truncate(40);
        flag = false;
        txt_RigID->setPalette(palHi);
        txt_RigID->setText(QString(txt));
    }
    else
    {
        txt_RigID->setPalette(palStd);
        txt_RigID->setText(QString(txt));
    }
    rigdef->rigID = txt.toStdString();

    /// checking fore triangle
    rigdef->foreI = readLength(txt_foreI);
    if (rigdef->foreI < 100) // height too small
    {
        flag = false;
        rigdef->foreI = 100;
        txt_foreI->setPalette(palLo);
        txt_foreI->setText(displayLength(rigdef->foreI));
        return flag;
    }
    else
    {
        txt_foreI->setPalette(palStd);
        txt_foreI->setText(displayLength(rigdef->foreI));
    }

    rigdef->foreJ = readLength(txt_foreJ);
    I = (long)(rigdef->foreI);
    J = (long)(rigdef->foreJ);
    if (( I / J )> 10)
    {
        flag = false;
        txt_foreI->setPalette(palHi);
        txt_foreJ->setPalette(palLo);
        txt_foreI->setText(displayLength(rigdef->foreI));
        txt_foreJ->setText(displayLength(rigdef->foreJ));
        return flag;
    }
    else if (( J / I )> 2)
    {
        flag = false;
        txt_foreJ->setPalette(palHi);
        txt_foreI->setPalette(palLo);
        txt_foreI->setText(displayLength(rigdef->foreI));
        txt_foreJ->setText(displayLength(rigdef->foreJ));
        return flag;
    }
    else
    {
        txt_foreJ->setPalette(palStd);
        txt_foreI->setPalette(palStd);
        txt_foreI->setText(displayLength(rigdef->foreI));
        txt_foreJ->setText(displayLength(rigdef->foreJ));
    }

    /// checking mast height
    rigdef->MHeight = readLength(txt_MH);
    L1 = (long)(rigdef->MHeight);

    if ( L1 < I )
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_MH->setPalette(palLo);
        rigdef->MHeight = I;
        return flag;
    }
    else if ( L1 > (1.8 * I))
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_MH->setPalette(palHi);
        rigdef->MHeight = ceil (1.8 * I);
        return flag;
    }
    else
    {
        txt_foreI->setPalette(palStd);
        txt_MH->setPalette(palStd);
    }
    txt_MH->setText(displayLength(rigdef->MHeight));

    L1 = (long)(rigdef->MHeight);

    /// checking mast cord
    rigdef->MCord = readLength(txt_MC);
    if (rigdef->MCord > (L1 / 10))
    {
        flag = false;
        txt_MC->setPalette(palHi);
        rigdef->MCord = floor(L1 / 10);
    }
    else if (rigdef->MCord < (L1 / 200))
    {
        flag = false;
        txt_MC->setPalette(palLo);
        rigdef->MCord = ceil(L1 / 200);
    }
    else
    {
        txt_MC->setPalette(palStd);
    }
    txt_MC->setText(displayLength(rigdef->MCord));

    /// checking mast width
    rigdef->MWidth = readLength(txt_MW);
    if (rigdef->MWidth > rigdef->MCord)
    {
        flag = false;
        txt_MC->setPalette(palRel);
        txt_MW->setPalette(palHi);
        rigdef->MWidth = rigdef->MCord;
    }
    else  if (rigdef->MWidth < (rigdef->MCord /5))
    {
        flag = false;
        txt_MC->setPalette(palRel);
        txt_MW->setPalette(palLo);
        rigdef->MWidth = ceil(rigdef->MCord /5);
    }
    else
    {
        txt_MC->setPalette(palStd);
        txt_MW->setPalette(palStd);
    }
    txt_MW->setText(displayLength(rigdef->MWidth));

    /// checking mast rake
    rigdef->MRakeM = readLength(txt_MRkM);
    if ( rigdef->MRakeM > L1 / 5)
    {
        flag = false;
        rigdef->MRakeM = floor( L1 / 5 );
        txt_MH->setPalette(palRel);
        txt_MRkM->setPalette(palHi);
    }
    else if ( rigdef->MRakeM < -L1 / 5)
    {
        flag = false;
        rigdef->MRakeM = -floor( L1 / 5 );
        txt_MH->setPalette(palRel);
        txt_MRkM->setPalette(palLo);
    }
    else
    {
        txt_MH->setPalette(palStd);
        txt_MRkM->setPalette(palStd);
    }
    txt_MRkM->setText(displayLength(rigdef->MRakeM));

    /// computing mast rake in degree
    rigdef->MRakeD = radiansToDegrees(atan2(rigdef->MRakeM ,rigdef->MHeight));
    lbl_MRkD->setText(QString::number(rigdef->MRakeD));

    /// computing mast base distance to stem
    rigdef->MBase = rigdef->foreJ - rigdef->MRakeM * (rigdef->foreI / rigdef->MHeight);

    /// checking mast round
    rigdef->MRnd = readLength(txt_MRnd);
    rigdef->MRndPos = spinBox_MRndPos->value();
    if ( rigdef->MRnd > L1/10 )
    {
        flag = false;
        rigdef->MRnd = floor( L1/10 );
        txt_MRnd->setPalette(palHi);
        txt_MRnd->setPalette(palHi);
    }
    else
    {
        txt_MH->setPalette(palStd);
        txt_MRnd->setPalette(palStd);
    }
    txt_MRnd->setText(displayLength(rigdef->MRnd));

    /// checking mainsail tack height
    rigdef->BAD = readLength(txt_BAD);
    if (rigdef->BAD < 0 )
    {
        flag = false;
        rigdef->BAD = 0;
        txt_BAD->setPalette(palLo);
    }
    else if ( rigdef->BAD > L1 / 2 )
    {
        flag = false;
        rigdef->BAD = int(L1 / 2);
        txt_BAD->setPalette(palHi);
        txt_MH->setPalette(palRel);
    }
    else
    {
        txt_BAD->setPalette(palStd);
        txt_MH->setPalette(palStd);
    }
    txt_BAD->setText(displayLength(rigdef->BAD));

    /// checking mainsail head height
    rigdef->HAD = readLength(txt_HAD);
    if (rigdef->HAD < rigdef->BAD +10 )
    {
        flag = false;
        rigdef->HAD = rigdef->BAD +10;
        txt_HAD->setPalette(palLo);
        txt_BAD->setPalette(palRel);
    }
    else if ( rigdef->HAD > L1 )
    {
        flag = false;
        rigdef->HAD = L1;
        txt_HAD->setPalette(palHi);
        txt_MH->setPalette(palRel);
    }
    else
    {
        txt_HAD->setPalette(palStd);
        txt_BAD->setPalette(palStd);
        txt_MH->setPalette(palStd);
    }
    txt_HAD->setText(displayLength(rigdef->HAD));

    ///computing and display main sail tack and luff data
    CRigWorker worker(*rigdef);
    rigdef->MStack = worker.mastCenter( rigdef->BAD ) + CVector3d(rigdef->MCord /2, 0, 0);
    rigdef->MShead = worker.mastCenter( rigdef->HAD ) + CVector3d(rigdef->MCord /2, 0, 0);

    CVector3d MSluff = CVector3d(rigdef->MShead - rigdef->MStack);

    // compute mainsail luff round and its position
    CPoint3d p1, p2;
    real h=0, rd1 = 0, rd2 = 0;
    unsigned int rdPos = 49;
    j = 5;
    do
    {
        j++;
        p1 = rigdef->MStack + MSluff*(real( j ) / 50);
        h = p1.y();
        p2 = worker.mastCenter( h ) + CVector3d(rigdef->MCord /2, 0, 0);
        rd2 = CVector3d(p1-p2).x();

        if (fabs(rd2) < fabs(rd1) )
            rdPos = (j - 1) * 2;
        else
            rd1 = rd2;
    }
    while ( rd1 == rd2 && j < 45);

    // display mainsail data
    lbl_MS_TackX->setText( displayLength(int(round(rigdef->MStack.x()))) );
    lbl_MS_TackY->setText( displayLength(int(rigdef->MStack.y())) );

    lbl_MS_LuffL->setText( displayLength(int(round(MSluff.length()))) );
    lbl_MS_Rake->setText( displayLength(int(round(MSluff.x()))) );

    lbl_MS_LuffR->setText( displayLength(round(rd1)) );
    lbl_MS_LuffRP->setText( QString::number( rdPos ) +" %" );

    /// reading number of spreaders
    rigdef->SPNB = spinBox_SPNB->value();
    n = rigdef->SPNB;

    /// checking shrouds height and shroud base width
    rigdef->CSH = readLength(txt_CSH);

    if ( rigdef->CSH > L1) // cap shroud above mast head
    {
        flag = false;
        txt_CSH->setPalette(palHi);
        txt_MH->setPalette(palRel);
        rigdef->CSH = L1;
        return flag;
    }
    else if ( rigdef->CSH < (0.75 * I))
    {
        flag = false;
        txt_CSH->setPalette(palLo);
        txt_foreI->setPalette(palRel);
        rigdef->CSH = ceil(0.75 * I);
        return flag;
    }
    else
    {
        txt_MH->setPalette(palStd);
        txt_foreI->setPalette(palStd);
        txt_CSH->setPalette(palStd);
    }
    txt_CSH->setText( displayLength(rigdef->CSH) );

    rigdef->LSB = readLength(txt_LSB);

    if ( rigdef->LSB < rigdef->CSH / ( 10 * (1 + real(n)) ) )
    {
        flag = false;
        txt_LSB->setPalette(palLo);
        txt_CSH->setPalette(palRel);
        rigdef->LSB = ceil( rigdef->CSH / ( 10 * (1 + real(n)) ) );
    }
    else if ( rigdef->LSB >  rigdef->CSH / (2 + real(n)) )
    {
        flag = false;
        txt_LSB->setPalette(palHi);
        txt_CSH->setPalette(palRel);
        rigdef->LSB = floor( rigdef->CSH / (2 + real(n)) );
    }
    else
    {
        txt_CSH->setPalette(palStd);
        txt_LSB->setPalette(palStd);
    }
    txt_LSB->setText( displayLength(rigdef->LSB) );

    rigdef->CSB = readLength(txt_CSB);

    if ( n == 0 )
    {
        rigdef->CSB = rigdef->LSB;
    }
    else if (rigdef->CSB < rigdef->LSB )
    {
        flag = false;
        txt_CSB->setPalette(palLo);
        txt_LSB->setPalette(palRel);
        rigdef->CSB = ceil( rigdef->LSB );
    }
    else if (rigdef->CSB > (2 * rigdef->LSB))
    {
        flag = false;
        txt_CSB->setPalette(palHi);
        txt_LSB->setPalette(palRel);
        rigdef->CSB = floor(2 * rigdef->LSB);
    }
    else
    {
        txt_CSB->setPalette(palStd);
        txt_LSB->setPalette(palStd);
    }
    txt_CSB->setText( displayLength(rigdef->CSB) );

    /// checking spreaders
    rigdef->SPH[0] = 0;
    rigdef->SPW[0] = rigdef->LSB;

    rigdef->SPH[1] = readLength(txt_SPH1);
    rigdef->SPH[2] = readLength(txt_SPH2);
    rigdef->SPH[3] = readLength(txt_SPH3);

    rigdef->SPW[1] = readLength(txt_SPW1);
    rigdef->SPW[2] = readLength(txt_SPW2);
    rigdef->SPW[3] = readLength(txt_SPW3);

    if ( n < 1 )
    {
        // no spreaders then reset to base of shrouds
        for (j = 1; j < 4; j++)
        {
            rigdef->SPH[j] = rigdef->SPH[0];
            rigdef->SPW[j] = rigdef->SPW[0];
        }
        txt_SPH1->setText( displayLength(rigdef->SPH[1]) );
        txt_SPH2->setText( displayLength(rigdef->SPH[2]) );
        txt_SPH3->setText( displayLength(rigdef->SPH[3]) );

        txt_SPW1->setText( displayLength(rigdef->SPW[1]) );
        txt_SPW2->setText( displayLength(rigdef->SPW[2]) );
        txt_SPW3->setText( displayLength(rigdef->SPW[3]) );
    }
    else
    {
        if ( rigdef->SPH[1] < rigdef->CSH / (2 + real(n)) )
        {
            flag = false;
            txt_CSH->setPalette(palRel);
            txt_SPH1->setPalette(palLo);
            rigdef->SPH[1] = ceil( rigdef->CSH / (2 + real(n)) );
        }
        else if ( rigdef->SPH[1] > ( rigdef->CSH / (0.5 + real(n)) ) )
        {
            flag = false;
            txt_CSH->setPalette(palRel);
            txt_SPH1->setPalette(palHi);
            rigdef->SPH[1] = floor( rigdef->CSH / (0.5 + real(n)) );
        }
        else
        {
            txt_CSH->setPalette(palStd);
            txt_SPH1->setPalette(palStd);
        }
        txt_SPH1->setText( displayLength(rigdef->SPH[1]) );

        L2 = (long) (rigdef->CSH - rigdef->SPH[1]);

        if ( n > 1 )
        {
            if ( rigdef->SPH[2] < ( rigdef->SPH[1] + L2 / (1 + real(n)) ) )
            {
                flag = false;
                txt_SPH1->setPalette(palRel);
                txt_SPH2->setPalette(palLo);
                rigdef->SPH[2] = ceil( rigdef->SPH[1] + L2 /(1 + real(n) ) );
            }
            else if ( rigdef->SPNB>1 && rigdef->SPH[2] > (rigdef->SPH[1] + L2 / (real(n) -.5)) )
            {
                flag = false;
                txt_SPH1->setPalette(palRel);
                txt_SPH2->setPalette(palHi);
                rigdef->SPH[2] = floor(rigdef->SPH[1] + L2 /(real(n) -.5) );
            }
            else
            {
                txt_SPH1->setPalette(palStd);
                txt_SPH2->setPalette(palStd);
            }
            txt_SPH2->setText( displayLength(rigdef->SPH[2]) );

            L2 = (long)(rigdef->CSH - rigdef->SPH[2]);

            if ( n > 2 )
            {
                if ( rigdef->SPH[3] < ( rigdef->SPH[2] + L2 / real(n) ) )
                {
                    flag = false;
                    txt_SPH2->setPalette(palRel);
                    txt_SPH3->setPalette(palLo);
                    rigdef->SPH[3] = ceil( rigdef->SPH[2] + L2 / real(n) );
                }
                else if ( rigdef->SPH[3] > ( rigdef->CSH + L2 / (real(n) -1.5) ) )
                {
                    flag = false;
                    txt_SPH2->setPalette(palRel);
                    txt_SPH3->setPalette(palHi);
                    rigdef->SPH[3] = floor( rigdef->CSH + L2/(real(n) -1.5) );
                }
                else
                {
                    txt_SPH2->setPalette(palStd);
                    txt_SPH3->setPalette(palStd);
                }
                txt_SPH3->setText( displayLength(rigdef->SPH[3]) );
            }
        }
    }
    // return flag = true  IF everything is OK
    return flag;
}


