/* Muistipelin paluu
 *
 * COMP.CS.110 K2021
 *
 */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QProgressBar>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Tarkistetaan, onko pelaajien ja korttien määrät oikeat ja jos ei, niin
    // tulostetaan virheilmoitus. Muutoin stacked-widget vaihtaa sivua
    // pelinäkymään.
    void onStartPushButtonClicked();

    // Kääntää kortin.
    void cardButtonClicked();

    // Kääntää kortit takaisin väärin päin.
    void turnButtons();

    // Alustaa tarvittavat muuttujat tyhjiksi ja vaihtaa aloitusnäkymään.
    void onRestartPushButtonClicked();

private:
    Ui::MainWindow *ui;

    const int MAX_PLAYERS = 8;
    const int MAX_PAIRS = 12;
    const int DELAY = 1000;
    const int MIN_VALID = 0;    // QIntValidatorin pienin
    const int MAX_VALID = 999;  // ja suurin arvo
    const QString INVALID_INPUT= "Virheellinen syöte: "
                           "\n- pelaajien määrä tulee olla väliltä "
                           "1-" + QString::number(MAX_PLAYERS) +
            "\n- parien määrä tulee olla väliltä 1-" +
            QString::number(MAX_PAIRS);

    QPushButton* open_card_;

    std::vector<QPushButton*> cards_;
    std::vector<char> letters_;

    int player_count_ = 0;
    int pair_count_ = 0;
    int seed_value_= 0;
    int rows_ = 1;
    int cols_ = 1;
    int cards_open_ = 0;

    std::vector<std::pair<QRadioButton*, QProgressBar*>> players_;
    std::vector<std::pair<QRadioButton*, QProgressBar*>>::iterator in_turn_;

    // Muuntaa annetun numeerisen merkkijonon vastaavaksi kokonaisluvuksi
    // kutsumalla stoi-funktiota.
    // Jos annettu merkkijono ei ole numeerinen, palauttaa 0, mikä
    // tuottaa myöhemmin virheen.
    int stoi_with_check(const std::string& str);

    // Antaa sellaiset tulon tekijät, jotka ovat
    // mahdollisimman lähellä toisiaan.
    void ask_product_and_calculate_factors(int& smaller_factor,
                                           int& bigger_factor);

    // Luo vektorin kortteihin tulevista kirjaimista ja sekoittaa ne.
    void randomize_cards();

    // Generoi oikean määrän push-buttoneita, eli kortteja, ja sijoittaa ne
    // näytölle.
    void init_board();

    // Generoi oikean määrän radio-buttoneita ja progress-bareja pelaajia
    // varten. Radio-button näyttää pelaajan vuoron ja nimen ja progress-bar
    // näyttää visuaalisesti korttien määrää.
    void init_players();

    // Tulostaa lopputilanteen eli voittajan/(voittajat tasatilanteessa) ja
    // tämän parimäärän.
    void end_game();
};
#endif // MAINWINDOW_HH
