#include "bmachinecontrol.h"
#include <QMessageBox>
#include <stdio.h>
#include <string.h>

BMachineControl::BMachineControl()
{

}

QString BMachineControl::getWMIHWInfo(int type)
{
    QString hwInfo;
    QStringList sqlCmd;
    sqlCmd.clear();
    sqlCmd << "SELECT MACAddress FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))";
    sqlCmd << "SELECT PNPDeviceID FROM Win32_DiskDrive WHERE( PNPDeviceID IS NOT NULL) AND (MediaType LIKE 'Fixed%')";
    sqlCmd << "SELECT SerialNumber FROM Win32_BaseBoard WHERE (SerialNumber IS NOT NULL)";
    sqlCmd << "SELECT ProcessorId FROM Win32_Processor WHERE (ProcessorId IS NOT NULL)";
    sqlCmd << "SELECT SerialNumber FROM Win32_BIOS WHERE (SerialNumber IS NOT NULL)";
    sqlCmd << "SELECT Product FROM Win32_BaseBoard WHERE (Product IS NOT NULL)";

    QStringList columnName;
    columnName.clear();
    columnName << "MACAddress";
    columnName << "PNPDeviceID";
    columnName << "SerialNumber";
    columnName << "ProcessorId";
    columnName << "SerialNumber";
    columnName << "Product";

    QAxObject *objIWbemLocator = new QAxObject("WbemScripting.SWbemLocator");
    QAxObject *objWMIService = objIWbemLocator->querySubObject("ConnectServer(QString&,QString&)",QString("."), QString("root\\cimv2"));
    QString query;
    if ( type < sqlCmd.size() ) {
        query = sqlCmd.at(type);
    }

    QAxObject *objInterList = objWMIService->querySubObject("ExecQuery(QString&))", query);
    QAxObject *enum1 = objInterList->querySubObject("_NewEnum");

    IEnumVARIANT *enumInterface = 0;
    enum1->queryInterface(IID_IEnumVARIANT, (void**)&enumInterface);
    enumInterface->Reset();

    for ( int i = 0; i < objInterList->dynamicCall("Count").toInt(); ++i )
    {
        VARIANT *theItem = (VARIANT*)malloc(sizeof(VARIANT));
        if ( enumInterface->Next(1, theItem, NULL) != S_FALSE )
        {
            QAxObject *item = new QAxObject((IUnknown *)theItem->punkVal);
            if (item) {
                if ( type<columnName.size() ) {
                    QByteArray datagram(columnName.at(type).toLatin1());
                    const char* tempConstChar = datagram.data();
                    hwInfo=item->dynamicCall(tempConstChar).toString();
                }
            }
        }
    }

    return hwInfo;
}


QString BMachineControl::getHDLogicalID()
{
    DWORD VolumeSerialNumber;
    GetVolumeInformation(L"C:\\", NULL, 0, &VolumeSerialNumber, NULL, NULL, NULL, 0);
    return QString::number(VolumeSerialNumber, 16).toUpper();
}

QString BMachineControl::getMac()
{
    QString macAddress;
    QList<QNetworkAddressEntry> lclInfAE;
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface iface, list)
    {
        if ( !(iface.humanReadableName().contains("VMware",Qt::CaseInsensitive)) &&
             !(iface.humanReadableName().contains("Tunnel",Qt::CaseInsensitive)) &&
             !(iface.humanReadableName().contains("Tunneling",Qt::CaseInsensitive)) &&
             !(iface.humanReadableName().contains("Loopback",Qt::CaseInsensitive)) &&
             !(iface.humanReadableName().contains("Pseudo",Qt::CaseInsensitive)) )
        {
            if ( iface.hardwareAddress() != "" ) {
                macAddress = iface.hardwareAddress().toUpper();
            }

        }
    }
    return macAddress;
}



QString BMachineControl::getInfo()
{
    QString info;
    for ( int i = 1; i != 6; ++i )
    {
        info.append(getWMIHWInfo(i));
    }
    info.append(getHDLogicalID());
    return GetMd5(info);
}

QString BMachineControl::getKey(QString machineinfo, QString ddMMyyyy, int days)
{
    QString originalStr120;
    if ( machineinfo.isEmpty() ) {
        originalStr120 = QString("machineinfo") + ddMMyyyy + QString::number(days);
    } else {
        originalStr120 = machineinfo + ddMMyyyy + QString::number(days);
    }
    QCryptographicHash sha1(QCryptographicHash::Sha1);

    QByteArray datagram(originalStr120.toLatin1());

    for ( int i = 0; i != datagram.size(); ++i )
    {
        datagram[i] = datagram[i] ^ 'q' ^ 'y';
    }

    const char* tempConstChar = datagram.data();
    sha1.addData(tempConstChar);

    QString activeCode = sha1.result().toHex();

    return activeCode;
}

bool BMachineControl::activeKey(QString key)
{
    QSettings *reg = new QSettings(kReg, QSettings::NativeFormat);

    QDateTime dtc = QDateTime::currentDateTime();
    QDateTime dtct = dtc;

    int daydelay = 2;
    for ( int d = 0; d != daydelay; ++d )
    {
        for ( int i = 0; i < kValidity.size(); ++i )
        {
            int validity = kValidity.at(i);
            QString k0 = getKey(getInfo(), dtct.date().toString("ddMMyyyy"), validity);
            if (k0 == key) {
                reg->setValue(kKey, key);
                reg->setValue(kDateTime0, dtct);
                reg->setValue(kDateTime1, dtc);
                reg->setValue(kDateTime2, dtc.addDays(validity));
                reg->setValue(kDays, validity);

                surplusDays = validity;

                copyToFile(reg, true);
                return true;
            }
        }
        dtct.addDays(-1);
    }
    return false;
}

void BMachineControl::copyToFile(QSettings* reg, bool magic)
{
    QFileInfo info(kFile);
    QString path = info.path();

    QDir dir(path);
    if ( !dir.exists() ) {
        dir.mkpath(path);
    }

    QFile f(kFile);
    f.open(QIODevice::ReadWrite);
    QDataStream out(&f);
    out << reg->value(kKey).toString();
    out << reg->value(kDateTime0).toDateTime();
    out << reg->value(kDateTime1).toDateTime();
    out << reg->value(kDateTime2).toDateTime();
    out << reg->value(kDays).toInt();

    if ( magic ) {
        out << ".";
    }

    f.close();
}

bool BMachineControl::initializeReg()
{
    QSettings *reg = new QSettings(kReg, QSettings::NativeFormat);

    QDateTime dtc = QDateTime::currentDateTime();

    QFileInfo info(kFile);

    if ( !reg->allKeys().contains(kDateTime0) ) {
        reg->setValue(kKey, "");
        reg->setValue(kDateTime0, dtc);
        reg->setValue(kDateTime1, dtc);
        reg->setValue(kDateTime2, dtc);
        reg->setValue(kDays, 0);

        copyToFile(reg, true);
        return true;
    } else {
        return false;
    }
}

bool BMachineControl::judgeFile()
{
    QSettings *reg = new QSettings(kReg, QSettings::NativeFormat);

    QFile f(kFile);
    f.open(QIODevice::ReadOnly);

    QString k;
    QDateTime dt0;
    QDateTime dt1;
    QDateTime dt2;
    int d;

    QDataStream in(&f);
    in >> k >> dt0 >> dt1 >> dt2 >> d;

    f.close();

    if (k != reg->value(kKey).toString() ||
            dt0 != reg->value(kDateTime0).toDateTime() ||
            dt1 != reg->value(kDateTime1).toDateTime() ||
            dt2 != reg->value(kDateTime2).toDateTime() ||
            d != reg->value(kDays).toInt()) {
        return false;
    } else {
        return true;
    }
}

bool BMachineControl::judgeDate()
{
    QSettings *reg = new QSettings(kReg, QSettings::NativeFormat);

    QDateTime dt1 = reg->value(kDateTime1).toDateTime();

    QDateTime dtc = QDateTime::currentDateTime();

    if ( dt1.secsTo(dtc) > 0 ) {
        return true;
    } else {
        return false;
    }
}

int BMachineControl::judgeKey()
{
    QSettings *reg = new QSettings(kReg, QSettings::NativeFormat);

    QDateTime dt0 = reg->value(kDateTime0).toDateTime();
    QDate d0 = dt0.date();
    QDateTime dt2 = reg->value(kDateTime2).toDateTime();
    int days = reg->value(kDays).toInt();
    QDateTime dtc = QDateTime::currentDateTime();


    QString k0 = getKey(getInfo(), d0.toString("ddMMyyyy"), days);

    if ( k0 == reg->value(kKey).toString() ) {
        if ( dtc.secsTo(dt2) > 0 ) {
            return dtc.daysTo(dt2);
        } else {
            return -1;
        }
    } else {
        return -2;
    }
}

void BMachineControl::refreshDT1()
{
    QSettings *reg = new QSettings(kReg, QSettings::NativeFormat);

    QDateTime dtc = QDateTime::currentDateTime();

    reg->setValue(kDateTime1, dtc);

    copyToFile(reg);
}

QString BMachineControl::GetMd5(const QString &value)
{
    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash(value.toUtf8(), QCryptographicHash::Md5);
    md5.append(bb.toHex());

    return md5;
}
