#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPoint>
#include <QList>
#include <QKeyEvent>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void moveDown(); // 自动下落

private:
    void initGame();
    void newPiece();  // 生成新方块
    bool checkCollision(const QList<QPoint> &piece, const QPoint &offset);
    void mergePiece(); // 方块落地，合并到背景
    void clearLines(); // 消行
    void gameOver();

private:
    // 游戏常量
    static const int BOARD_WIDTH = 10;
    static const int BOARD_HEIGHT = 20;
    static const int BLOCK_SIZE = 30;

    // 当前方块
    QList<QPoint> currentPiece;
    QPoint currentPos;
    int currentColor;

    // 背景格子（已落地的方块）
    QList<QList<int>> board; // 0=空，1~7=颜色

    QTimer *timer;
    bool isGameOver;
    int score;
};

#endif // WIDGET_H