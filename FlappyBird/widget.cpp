#include "widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(400, 600);
    initGame();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::updateGame);
    timer->start(16);
}

void Widget::initGame()
{
    birdY = height() / 2;
    gameOver = false;
    score = 0;
    speed = 2;
    pipes.clear();

    // 去掉重力，改用上下控制
    upPress = false;
    downPress = false;
}

void Widget::createPipe()
{
    if (pipes.isEmpty() || pipes.last().x < width() - 180) {
        pipes.append({width(), 150 + rand() % 250});
    }
}

void Widget::updateGame()
{
    if (gameOver) return;

    // ======================
    // 纯手动控制！无重力！
    // ======================
    const int MOVE_SPEED = 5;
    if (upPress) {
        birdY -= MOVE_SPEED;
    }
    if (downPress) {
        birdY += MOVE_SPEED;
    }

    // 边界限制
    if (birdY < 0) birdY = 0;
    if (birdY + birdSize > height()) birdY = height() - birdSize;

    // 管道移动
    for (int i = 0; i < pipes.size(); i++) {
        pipes[i].x -= speed;
        if (pipes[i].x < -80) {
            pipes.removeAt(i);
            score++;
        }
    }

    createPipe();

    // 碰撞
    if (checkCollision()) {
        gameOver = true;
    }

    update();
}

bool Widget::checkCollision()
{
    for (auto &p : pipes) {
        if (80 + birdSize > p.x && 80 < p.x + 80) {
            if (birdY < p.gapY - 70 || birdY + birdSize > p.gapY + 70) {
                return true;
            }
        }
    }
    return false;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient bg(0,0,0,height());
    bg.setColorAt(0, QColor(110,180,255));
    bg.setColorAt(1, QColor(180,220,255));
    p.fillRect(rect(), bg);

    p.setBrush(QColor(80,180,80));
    p.setPen(Qt::black);
    for (auto& pipe : pipes) {
        p.drawRect(pipe.x, 0, 80, pipe.gapY - 70);
        p.drawRect(pipe.x, pipe.gapY + 70, 80, height());
    }

    p.setBrush(QColor(255,220,0));
    p.drawEllipse(80, birdY, birdSize, birdSize);

    p.setPen(Qt::black);
    p.setFont(QFont("Arial",24,QFont::Bold));
    p.drawText(20,40,"Score: "+QString::number(score));

    if (gameOver) {
        p.setPen(Qt::red);
        p.setFont(QFont("Arial",40,QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER");
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Up)
        upPress = true;
    if (e->key() == Qt::Key_Down)
        downPress = true;

    // 空格也能当跳跃用
    if (e->key() == Qt::Key_Space) {
        if (gameOver)
            initGame();
        else
            birdY -= 8;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Up)
        upPress = false;
    if (e->key() == Qt::Key_Down)
        downPress = false;
}