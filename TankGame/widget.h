#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QList>

class Widget : public QWidget
{
    Q_OBJECT
public:
    enum GameMode { SINGLE, TWO_PLAYER };
    Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

private slots:
    void updateGame();

private:
    struct Tank {
        int x, y, dir, w, h, speed;
        bool fire;
    };

    struct Bullet {
        int x, y, dir;
        int owner; // 0=玩家1 / 1=玩家2 / 2=敌人
    };

    struct Wall {
        int x, y, w, h;
    };

    void initGame(GameMode mode);
    void fire(Tank &tank, int owner);
    void spawnEnemy();
    void checkCollisions();
    void drawTank(QPainter &p, Tank &tank, QColor color);

    QTimer *timer;
    GameMode currentMode;

    Tank player1;
    Tank player2;
    QList<Tank> enemies;
    QList<Bullet> bullets;
    QList<Wall> walls;

    bool p1_up, p1_down, p1_left, p1_right;
    bool p2_w, p2_s, p2_a, p2_d;
    bool gameOver;
    int winner;
};

#endif // WIDGET_H