#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QPoint>
#include <QKeyEvent>
#include <QPixmap>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop(); // 游戏主循环

private:
    void initGame();
    void createEnemy();
    void moveBullets();
    void moveEnemies();
    void checkCollisions();
    void gameOver();

private:
    static const int GAME_WIDTH = 480;
    static const int GAME_HEIGHT = 640;
    static const int PLANE_SPEED = 5;
    static const int BULLET_SPEED = 8;
    static const int ENEMY_SPEED = 2;

    int difficulty;   // 难度 1~3
    void setDifficulty(int level); // 设置难度
    int enemyInterval;  // 敌机生成间隔
    // 玩家飞机
    QPoint planePos;
    QList<Qt::Key> keys; // 按下的按键

    // 子弹 & 敌机
    QList<QPoint> bullets;
    QList<QPoint> enemies;

    QTimer *timer;
    bool isGameOver;
    int score;

    QPixmap planePix;
    QPixmap enemyPix;

    // 爆炸效果
    struct Explosion {
        QPoint pos;
        int frame;  // 动画帧（0~5）
    };
    QList<Explosion> explosions;
    QPixmap explosionPix;  // 爆炸图片
};

#endif // WIDGET_H