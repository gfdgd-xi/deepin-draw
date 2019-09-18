#include "cprintmanager.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QDebug>

DWIDGET_USE_NAMESPACE

CPrintManager::CPrintManager(QObject *parent)
    : QObject (parent)
{

}

CPrintManager::~CPrintManager()
{

}

void CPrintManager::showPrintDialog(const QPixmap &pixmap, DWidget *widget)
{
    m_pixMap = pixmap;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QPrinter::Custom);
    printer.setPaperSize(QSize(m_pixMap.width(), m_pixMap.height()),
                         QPrinter::DevicePixel);
    printer.setPageMargins(0., 0., 0., 0., QPrinter::DevicePixel);
    printer.setFullPage(true);

    QString desktopDir = QStandardPaths::writableLocation(
                             QStandardPaths::DesktopLocation);
    QString filePath = QString("%1/%2.pdf").arg(desktopDir).arg(tr("DeepIn"));

    if (QFileInfo(filePath).exists()) {
        int num = 0;
        while (QFileInfo(filePath).exists()) {
            num++;
            filePath = QString("%1/%2_%3.pdf").arg(desktopDir).arg(tr("DeepIn")).arg(num);
        }
    }
    printer.setOutputFileName(filePath);


    QPrintPreviewDialog preview(&printer, widget);
    preview.setFixedSize(1000, 600);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slotPrintPreview(QPrinter *)));
    preview.exec ();
}


void CPrintManager::slotPrintPreview(QPrinter *printerPixmap)
{
    QPainter painter(printerPixmap);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QRectF drawRectF;
    QRect wRect;

    wRect = printerPixmap->pageRect();
    if (m_pixMap.width() > wRect.width() || m_pixMap.height() > wRect.height()) {
        m_pixMap = m_pixMap.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    drawRectF = QRectF(qreal(wRect.width() - m_pixMap.width()) / 2,
                       qreal(wRect.height() - m_pixMap.height()) / 2,
                       m_pixMap.width(), m_pixMap.height());
    painter.drawPixmap(drawRectF.x(), drawRectF.y(), m_pixMap.width(),
                       m_pixMap.height(), m_pixMap);
}