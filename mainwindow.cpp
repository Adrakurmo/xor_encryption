#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QByteArray>
#include <string>
#include <zlib.h>

std::string compress_text(const std::string& str, int compression_level = Z_BEST_COMPRESSION);
std::string decompress_text(const std::string& str);
QByteArray xor_encrypt(const QByteArray& data, const QByteArray& key);
std::string generate_xor_key(size_t length);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::string compress_text(const std::string& str, int compression_level){
    z_stream zs{};
    deflateInit(&zs, compression_level);

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(str.data()));
    zs.avail_in = str.size();

    int ret = 0;
    char outbuffer[32768];
    std::string outstr;

    do{
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof (outbuffer);

        ret = deflate(&zs, Z_FINISH);
        if (outstr.size() < zs.total_out) outstr.append(outbuffer, zs.total_out - outstr.size());
    } while (ret == Z_OK);

    deflateEnd(&zs);
    return outstr;
}

std::string decompress_text(const std::string& str) {
    z_stream zs{};
    inflateInit(&zs);

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(str.data()));
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);
        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) throw std::runtime_error("Error przy dekompresji xd: " + std::to_string(ret));

    return outstring;
}

std::string generate_xor_key(size_t length) {
    std::string key;
    key.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        key += static_cast<char>(rand() % 256);
    }
    return key;
}

QByteArray xor_encrypt(const QByteArray& data, const QByteArray& key) {
    QByteArray result = data;
    for (int i = 0; i < data.size(); ++i) {
        result[i] = result[i] ^ key[i % key.size()];
    }
    return result;
}




void MainWindow::on_toCompressTE_textChanged()
{
    std::string original = ui->toCompressTE->toPlainText().toStdString();

    if (original == ""){
        ui->sizeAfterComprTB->setText("");
        ui->sizeBeforeComprTB->setText("");
        ui->compressedTB->setText("");
        return;
    }

    ui->sizeBeforeComprTB->setText(QString::number(original.size()));

    std::string compressed = compress_text(original);

    QByteArray compressed_b64 = QByteArray::fromRawData(compressed.data(), compressed.size()).toBase64();
    ui->compressedTB->setText(QString::fromUtf8(compressed_b64));

    ui->sizeAfterComprTB->setText(QString::number(compressed.size()));
}

void MainWindow::on_genNewKeyPB_clicked()
{
    std::string new_key = generate_xor_key(ui->keyLengthSP->text().toInt());
    QByteArray key_b64 = QByteArray::fromRawData(new_key.data(), new_key.size()).toBase64();
    ui->keyTB->setText(QString::fromUtf8(key_b64));
    ui->xorEncyptTB->setText("");
}

void MainWindow::on_xorEncyptPB_clicked()
{

    if (ui->xorEncyptTB->toPlainText().isEmpty()){
        ui->xorEncyptTB->setText(ui->compressedTB->toPlainText());
    }


    QByteArray to_encrypt_raw = ui->xorEncyptTB->toPlainText().toUtf8();
    QByteArray key_raw = ui->keyTB->toPlainText().toUtf8();

    if (key_raw.isEmpty()) {
        QMessageBox::warning(this, "Hello", "What about key? huh??????");
        return;
    }

    if (ui->compressedTB->toPlainText().toUtf8().isEmpty()){
        QMessageBox::warning(this, "Heeey", "You have a key but how are you exactly planning on using it without.. I don't know maybe a PLAINTEXT?!");
        return;
    }

    QByteArray encrypted = xor_encrypt(to_encrypt_raw, key_raw);

    ui->xorEncyptTB->setText(
                xor_encrypt(
                    QByteArray::fromBase64(to_encrypt_raw),
                    QByteArray::fromBase64(key_raw)).toBase64()
                );

//    TODO xd
//    QByteArray tmp_b64 = ui->xorEncyptTB->toPlainText().toUtf8();
//    QByteArray compressed_raw = QByteArray::fromBase64(tmp_b64);
//    std::string decompressed = decompress_text(std::string(compressed_raw.data(), compressed_raw.size()));
//    ui->decompressedTB->setText(QString::fromStdString(decompressed));
}
