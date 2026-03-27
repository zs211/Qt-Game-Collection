#include "widget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QFont>

const QList<QList<QPoint>> shapes = {
    { {0,0}, {1,0}, {2,0}, {3,0} },
    { {0,0}, {1,0}, {0,1}, {1,1} },
    { {0,1}, {1,0}, {1,1}, {2,1} },
    { {0,0}, {0,1}, {0,2}, {1,2} },
    { {1,0}, {1,1}, {1,2}, {0,2} },
    { {1,0}, {2,0}, {0,1}, {1,1} },
    { {0,0}, {1,0}, {1,1}, {2,1} }
};

const QList<QColor> colors = {
    Qt::cyan,
    Qt::yellow,
    Qt::magenta,
    QColor(255, 165, 0),
    Qt::blue,
    Qt::green,
    Qt::red
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE);
    initGame();
}

Widget::~Widget()
{
    delete timer;
}

void Widget::initGame()
{
    board.clear();
    for(int y=0; y<BOARD_HEIGHT; y++){
        QList<int> row;
        for(int x=0; x<BOARD_WIDTH; x++)
            row << 0;
        board << row;
    }

    isGameOver = false;
    score = 0;
    setWindowTitle(QString("俄罗斯方块 | Score: %1").arg(score));

    newPiece();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::moveDown);
    timer->start(800);
}

void Widget::newPiece()
{
    int type = QRandomGenerator::global()->bounded(7);
    currentPiece = shapes[type];
    currentColor = type;
    currentPos = QPoint(BOARD_WIDTH/2 - 2, 0);

    if(checkCollision(currentPiece, currentPos)){
        gameOver();
    }
}

bool Widget::checkCollision(const QList<QPoint> &piece, const QPoint &offset)
{
    for(const QPoint &p : piece){
        int x = offset.x() + p.x();
        int y = offset.y() + p.y();

        if(x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT)
            return true;
        if(y >= 0 && board[y][x] != 0)
            return true;
    }
    return false;
}

void Widget::mergePiece()
{
    for(const QPoint &pt : currentPiece){
        int x = currentPos.x() + pt.x();
        int y = currentPos.y() + pt.y();
        if(y >= 0)
            board[y][x] = currentColor + 1;
    }
    clearLines();
    newPiece();
}

void Widget::clearLines()
{
    int linesCleared = 0;
    for(int y=BOARD_HEIGHT-1; y>=0; y--){
        bool full = true;
        for(int x=0; x<BOARD_WIDTH; x++){
            if(board[y][x] == 0){
                full = false;
                break;
            }
        }
        if(full){
            for(int yy = y; yy > 0; yy--){
                board[yy] = board[yy-1];
            }
            board[0] = QList<int>(BOARD_WIDTH, 0);
            y++;
            linesCleared++;
        }
    }
    if(linesCleared > 0){
        score += linesCleared * 100;
        setWindowTitle(QString("俄罗斯方块 | Score: %1").arg(score));
    }
}

void Widget::gameOver()
{
    isGameOver = true;
    timer->stop();
    update();
}

void Widget::moveDown()
{
    if(isGameOver) return;

    QPoint newPos = currentPos + QPoint(0, 1);
    if(!checkCollision(currentPiece, newPos)){
        currentPos = newPos;
    } else {
        mergePiece();
    }
    update();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(40,40,40));

    for(int y=0; y<BOARD_HEIGHT; y++){
        for(int x=0; x<BOARD_WIDTH; x++){
            if(board[y][x] > 0){
                p.setBrush(colors[board[y][x]-1]);
                p.setPen(Qt::black);
                p.drawRect(x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE-1, BLOCK_SIZE-1);
            }
        }
    }

    p.setBrush(colors[currentColor]);
    p.setPen(Qt::black);
    for(const QPoint &pt : currentPiece){
        int x = currentPos.x() + pt.x();
        int y = currentPos.y() + pt.y();
        if(y >= 0)
            p.drawRect(x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE-1, BLOCK_SIZE-1);
    }

    if(isGameOver){
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 30, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER");
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if(isGameOver){
        if(e->key() == Qt::Key_Space)
            initGame();
        return;
    }

    switch(e->key()){
    case Qt::Key_Left: {
        QPoint newPos = currentPos + QPoint(-1, 0);
        if(!checkCollision(currentPiece, newPos))
            currentPos = newPos;
        break;
    }
    case Qt::Key_Right: {
        QPoint newPos = currentPos + QPoint(1, 0);
        if(!checkCollision(currentPiece, newPos))
            currentPos = newPos;
        break;
    }
    case Qt::Key_Down: {
        QPoint newPos = currentPos + QPoint(0, 1);
        if(!checkCollision(currentPiece, newPos))
            currentPos = newPos;
        break;
    }
    case Qt::Key_Up: {
        QList<QPoint> rotated;
        for(const QPoint &pt : currentPiece){
            rotated << QPoint(pt.y(), -pt.x());
        }
        if(!checkCollision(rotated, currentPos))
            currentPiece = rotated;
        break;
    }
    case Qt::Key_Space:
        while(!checkCollision(currentPiece, currentPos + QPoint(0,1))){
            currentPos += QPoint(0,1);
        }
        mergePiece();
        break;
    }
    update();
}