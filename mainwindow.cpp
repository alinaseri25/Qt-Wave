#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Txt_FileAddress->setText(QString("C:\\1.wav"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Btn_OpenFile_clicked()
{
    on_Btn_CloseFile_clicked();
    Header = new WaveHeader();
    uint32_t ReadSize;
    waveFile.setFileName(ui->Txt_FileAddress->text());
    if(!waveFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::about(this,QString("Error"),QString("Can't Open This File !"));
        return;
    }
    ReadSize = waveFile.read((char *)Header,sizeof (WaveHeader));

    if(ReadSize != sizeof (WaveHeader))
    {
        QMessageBox::about(this,QString("Error"),QString("Can't Read This File !"));
        return;
    }

    ui->Txt_Out->append(QString("File Size : %1 Byte").arg(waveFile.size()));

    ui->Txt_Out->append(QString("---------------"));

    ui->Txt_Out->append(QString("RIFF Header : %1%2%3%4").arg(QString(QChar(Header->RIFF[0]))).arg(QString(QChar(Header->RIFF[1])))
                            .arg(QString(QChar(Header->RIFF[2]))).arg(QString(QChar(Header->RIFF[3]))));
    ui->Txt_Out->append(QString("RIFF Chunk Size : %1").arg(Header->ChunkSize));
    ui->Txt_Out->append(QString("Wave Header : %1%2%3%4").arg(QString(QChar(Header->WAVE[0]))).arg(QString(QChar(Header->WAVE[1])))
                            .arg(QString(QChar(Header->WAVE[2]))).arg(QString(QChar(Header->WAVE[3]))));

    ui->Txt_Out->append(QString("---------------"));

    ui->Txt_Out->append(QString("FMT Header : %1%2%3%4").arg(QString(QChar(Header->fmt[0]))).arg(QString(QChar(Header->fmt[1])))
                            .arg(QString(QChar(Header->fmt[2]))).arg(QString(QChar(Header->fmt[3]))));
    ui->Txt_Out->append(QString("Data size : %1").arg(Header->Subchunk1Size));
    ui->Txt_Out->append(QString("Sampling Rate : %1").arg(Header->SamplesPerSec));
    ui->Txt_Out->append(QString("Number of bits used : %1").arg(Header->bitsPerSample));
    ui->Txt_Out->append(QString("Number of channels : %1").arg(Header->NumOfChan));
    ui->Txt_Out->append(QString("Number of bytes per second : %1").arg(Header->bytesPerSec));
    ui->Txt_Out->append(QString("Data length : %1").arg(Header->Subchunk2Size));
    ui->Txt_Out->append(QString("Audio Format : %1").arg(Header->AudioFormat));

    ui->Txt_Out->append(QString("---------------"));

    ui->Txt_Out->append(QString("Block align : %1").arg(Header->blockAlign));
    ui->Txt_Out->append(QString("Data string : %1%2%3%4").arg(QString(QChar(Header->Subchunk2ID[0]))).arg(QString(QChar(Header->Subchunk2ID[1])))
                            .arg(QString(QChar(Header->Subchunk2ID[2]))).arg(QString(QChar(Header->Subchunk2ID[3]))));

    ui->Txt_Out->append(QString("---------------"));
    delete(Header);
}

void MainWindow::on_Btn_CloseFile_clicked()
{
    if(waveFile.isOpen())
    {
        waveFile.close();
        ui->Txt_Out->append(QString("File Closed !"));
        ui->Txt_Out->append(QString("---------------"));
    }
}

void MainWindow::on_Btn_Create_clicked()
{
    on_Btn_CloseFile_clicked();
    Header = new WaveHeader();
    waveFile.setFileName(ui->Txt_FileAddress->text());

    if(!waveFile.open(QIODevice::ReadWrite))
    {
        QMessageBox::about(this,QString("Error"),QString("Can't Open This File !"));
        return;
    }
    Header->RIFF[0] = 'R';
    Header->RIFF[1] = 'I';
    Header->RIFF[2] = 'F';
    Header->RIFF[3] = 'F';

    Header->WAVE[0] = 'W';
    Header->WAVE[1] = 'A';
    Header->WAVE[2] = 'V';
    Header->WAVE[3] = 'E';

    Header->fmt[0] = 'f';
    Header->fmt[1] = 'm';
    Header->fmt[2] = 't';
    Header->fmt[3] = ' ';

    Header->Subchunk1Size = 16;
    Header->AudioFormat = WAVE_FORMAT_PCM;
    Header->SamplesPerSec = 8000;
    //Header->NumOfChan = 2;
    Header->NumOfChan = 1;
    Header->bitsPerSample = 16;
    Header->bytesPerSec = (Header->SamplesPerSec * Header->NumOfChan * Header->bitsPerSample) / 8;
    Header->blockAlign = 4;

    Header->blockAlign = 4;
    Header->Subchunk2ID[0] = 'd';
    Header->Subchunk2ID[1] = 'a';
    Header->Subchunk2ID[2] = 't';
    Header->Subchunk2ID[3] = 'a';

    constexpr double two_pi = 6.283185307179586476925286766559;
    constexpr double max_amplitude = 32767;  // "volume"

    double frequency = 500;  // middle C
    double seconds   = 10;      // time

    int N = Header->SamplesPerSec * seconds;  // total number of samples

    waveFile.seek(sizeof(WaveHeader));
    uint16_t data_to_write;
    char data[4];
    for (int n = 0; n < N; n++)
    {
        double amplitude = max_amplitude;//(double)n / N * max_amplitude;
        double value     = sin( (two_pi * n * frequency) / Header->SamplesPerSec );
        data_to_write = (amplitude  * value);
        data[0] = (char)(data_to_write & 0xFF);
        data[1] = (char)((data_to_write >> 8 ) & 0xFF);
        data_to_write = (amplitude  * value);//((max_amplitude - amplitude) * value);
        data[2] = (char)(data_to_write & 0xFF);
        data[3] = (char)((data_to_write >> 8 ) & 0xFF);
        //waveFile.write(data,4);
        waveFile.write(data,2);
    }

    waveFile.seek(0);
    //Header->ChunkSize = (Header->bytesPerSec * seconds) + 36 ;
    Header->ChunkSize = waveFile.size() - 8;
    Header->Subchunk2Size = Header->ChunkSize - 20;
    waveFile.write((char *)Header,sizeof(WaveHeader));

    ui->Txt_Out->append(QString("Wave File Created !"));
    ui->Txt_Out->append(QString("---------------"));

    delete(Header);
}
