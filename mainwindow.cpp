/* Muistipelin paluu
 *
 * COMP.CS.110 K2021
 *
 */

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QIntValidator>
#include <algorithm>
#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIntValidator* max_input = new QIntValidator(MIN_VALID, MAX_VALID, this);
    ui->playerCountLineEdit->setValidator(max_input);
    ui->pairCountLineEdit->setValidator(max_input);
    ui->seedLineEdit->setValidator(max_input);

    // Aloitetaan aina ensimmäiseltä sivulta.
    ui->stackedWidget->setCurrentIndex(0);

    cards_open_ = 0;
    open_card_ = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;

    for (QPushButton* card: cards_)
    {
        delete card;
    }
    for (std::pair<QRadioButton*,QProgressBar*> player: players_)
    {
        delete player.first;
        delete player.second;
    }
}

void MainWindow::cardButtonClicked()
{
    // Luodaan uusi osoittaja nappulaan, jota painettiin.
    QPushButton* pressedButton = qobject_cast<QPushButton*>(sender());

    if (cards_open_ != 2 and pressedButton != open_card_)
    {
        pressedButton->setStyleSheet("color: black");

        ++cards_open_;

        if (!open_card_)
        {
            open_card_ = pressedButton;
        }
        else
        {
            if (pressedButton->text() == open_card_->text())
            {
                pressedButton->hide();
                open_card_->hide();
                in_turn_->second->setValue(in_turn_->second->value()+1);
                open_card_ = nullptr;
                cards_open_ = 0;

                // Tarkistetaan, onko kaikki kortit löydetty.
                for (QPushButton* button: cards_)
                {
                    if (button->isVisible())
                    {
                        return;
                    }
                }
                end_game();
            }
        }
        if (cards_open_ == 2)
        {
            ui->quitPushButton->setEnabled(false);
            ui->restartPushButton->setEnabled(false);

            QTimer::singleShot(DELAY, this, SLOT(turnButtons()));
        }
    }
}

void MainWindow::turnButtons()
{
    for (QPushButton* button: cards_)
    {
        button->setStyleSheet("color: rgba(0,0,0,0);"
                              "background-color: green;");
        open_card_ = nullptr;
    }
    cards_open_ = 0;

    ++in_turn_;
    if (in_turn_ == players_.end())
    {
        in_turn_ = players_.begin();
    }
    in_turn_->first->setChecked(true);

    ui->quitPushButton->setEnabled(true);
    ui->restartPushButton->setEnabled(true);
}

void MainWindow::onStartPushButtonClicked()
{
    player_count_ = stoi_with_check(
                ui->playerCountLineEdit->text().toStdString());
    pair_count_ = stoi_with_check(ui->pairCountLineEdit->text().toStdString());
    seed_value_ = stoi_with_check(ui->seedLineEdit->text().toStdString());

    if (player_count_ >= 1 and player_count_ <= MAX_PLAYERS and
            pair_count_ >= 1 and pair_count_ <= MAX_PAIRS)
    {
        ask_product_and_calculate_factors(rows_, cols_);

        ui->stackedWidget->setCurrentIndex(1);
        init_board();
        init_players();
    }
    else
    {
        ui->startTextBrowser->setText(INVALID_INPUT);
    }
}

void MainWindow::onRestartPushButtonClicked()
{
    open_card_ = nullptr;
    letters_.clear();

    for (QPushButton* card: cards_)
    {
        delete card;
    }
    cards_.clear();

    for (std::pair<QRadioButton*,QProgressBar*> player: players_)
    {
        delete player.first;
        delete player.second;
    }
    players_.clear();

    player_count_ = 0;
    pair_count_ = 0;
    rows_ = 0;
    cols_ = 0;
    cards_open_ = 0;

    ui->startTextBrowser->setText("");
    ui->endTextBrowser->setText("");
    ui->stackedWidget->setCurrentIndex(0);
}

int MainWindow::stoi_with_check(const std::string& str)
{
    bool is_numeric = true;
    if (!str.empty())
    {
        for (unsigned int i = 0; i < str.length(); ++i)
        {
            if (not isdigit(str.at(i)))
            {
                is_numeric = false;
                break;
            }
        }
        if (is_numeric)
        {
            return stoi(str);
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

void MainWindow::ask_product_and_calculate_factors(
        int& smaller_factor, int& bigger_factor)
{
    int product = 0;

    while (not (product > 0 and product % 2 == 0))
    {
        // Korttien määrä saadaan kertomalla parien määrä kahdella
        product = 2 * pair_count_;
    }
    for (int i = 1; i * i <= product; ++i)
    {
        if (product % i == 0)
        {
            smaller_factor = i;
        }
    }
    bigger_factor = product / smaller_factor;
}

void MainWindow::randomize_cards()
{
    char letter = 'A';
    int seed = clock();

    if (seed_value_ > 0)
    {
        seed = seed_value_;
    }

    std::default_random_engine random_eng(seed);

    // Loopataan merkkien läpi käyttäen aakkosten ASCII-arvoja. Jokaista
    // merkkiä tulee kaksi kappaletta.
    while (letter < 65+pair_count_)
    {
        letters_.push_back(letter);
        letters_.push_back(letter);
        ++letter;
    }
    std::shuffle(letters_.begin(), letters_.end(), random_eng);
}

void MainWindow::init_board()
{
    randomize_cards();
    int letter_index = 0;

    for (int i = 0; i <= rows_-1; ++i)
    {
        for (int j = 0; j <= cols_-1; ++j)
        {
            QChar letter = letters_.at(letter_index);

            QPushButton* pushButton = new QPushButton(letter, this);
            pushButton->setFixedSize(50, 100);
            pushButton->setStyleSheet("color: rgba(0,0,0,0);"
                                      "background-color: green;");

            cards_.push_back(pushButton);
            ui->gameGridLayout->addWidget(pushButton, i, j);

            connect(pushButton, &QPushButton::clicked, this,
                    &MainWindow::cardButtonClicked);

            ++letter_index;
        }
    }
}

void MainWindow::init_players()
{
    QString player_name = "Pelaaja ";

    players_ = {};

    for (int i = 1; i <= player_count_; ++i)
    {
        QRadioButton* radioButton = new QRadioButton(
                    player_name+QString::number(i), this);
        QProgressBar* progressBar = new QProgressBar(this);
        players_.push_back({radioButton, progressBar});

        radioButton->setEnabled(false);
        radioButton->setStyleSheet("color: black");
        progressBar->setMaximum(pair_count_);
        progressBar->setValue(0);
        progressBar->setFormat("%v paria");
        ui->playersGridLayout->addWidget(radioButton, i, 0);
        ui->playersGridLayout->addWidget(progressBar, i, 1);
    }
    in_turn_ = players_.begin();
    in_turn_->first->setChecked(true);
}

void MainWindow::end_game()
{
    // Vektoriin winner lisätään kaikki, joilla on sama määrä pisteitä. Jos
    // seuraavalla pelaajalla on kuitenkin enemmän pisteitä kuin listassa
    // (ensimmäisenä) olevalla, listasta poistetaan muut ja lisätään vain uusi
    // pelaaja.
    std::vector<std::pair<QRadioButton*, QProgressBar*>> winners = {};
    QString winner_text = "Voittaja(t) on:\n";

    for (std::pair<QRadioButton*, QProgressBar*> player: players_)
    {
        if (winners.empty() or
                player.second->value() == winners.at(0).second->value())
        {
            winners.push_back(player);
        }
        else if (player.second->value() > winners.at(0).second->value())
        {
            winners.clear();
            winners.push_back(player);
        }
    }

    for (std::pair<QRadioButton*, QProgressBar*> winner: winners)
    {
        winner_text += winner.first->text() + ", joka sai " +
                QString::number(winner.second->value()) + " paria!\n";
    }
    ui->endTextBrowser->setText(winner_text);
}

