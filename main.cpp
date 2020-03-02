#include <QCoreApplication>
#include <QStringList>
#include "default-values.h"
#include <QDebug>

bool debugFlag = false;
bool checkFlag = false;
bool newFlag = false;
QString iniFile;
QString halFile;

static void print_help()
{
    QString msg = QString("Usage: %1 [%2] [%3|%4] [%5CONFIGFILE] [%6HALFILE]")
            .arg(APP_TARGET)
            .arg(KEY_DEBUG)
            .arg(KEY_CHECK)
            .arg(KEY_NEW)
            .arg(KEY_INIFILE)
            .arg(KEY_HALFILE);
    qDebug() << "Version:" << APP_VERSION;
    qDebug() << msg;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    if (QCoreApplication::arguments().count() == 1) {
        print_help();
        return 0;
    }

    int keys = 1;
    foreach (QString s,QCoreApplication::arguments()) {
        if (s.toLower() == KEY_DEBUG) {
            keys++;
            debugFlag = true;
        }

        if (s.toLower() == KEY_CHECK) {
            keys++;
            checkFlag = true;
        }

        if (s.toLower() == KEY_NEW) {
            keys++;
            newFlag =true;
        }

        if (s.toLower().startsWith(KEY_INIFILE)) {
            keys++;
            iniFile = s;
        }

        if (s.toLower().startsWith(KEY_HALFILE)) {
            keys++;
            halFile = s;
        }
    }

    if ((newFlag && checkFlag) || (iniFile.isEmpty())
            || (keys != QCoreApplication::arguments().count())) {
        qDebug() << "Arguments are wrong or missing!";
        print_help();
        goto end;
    }

end:
    return 0;
}
