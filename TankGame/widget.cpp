#include "widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(800, 600);
    initGame(SINGLE); // 默认单人模式

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::updateGame);
    timer->start(16);
}

void Widget::initGame(GameMode mode)
{
    currentMode = mode;
    gameOver = false;
    winner = -1;
    bullets.clear();
    enemies.clear();

    // 玩家1（绿色）
    player1 = {200, 500, 0, 40, 40, 5, false};
    p1_up = p1_down = p1_left = p1_right = false;

    // 玩家2（蓝色，双人模式启用）
    player2 = {600, 500, 0, 40, 40, 5, false};
    p2_w = p2_s = p2_a = p2_d = false;

    // 墙壁
    walls.clear();
    for (int i=0; i<12; i++) walls.append({100+i*50, 300, 40, 20});
    for (int i=0; i<9; i++)  walls.append({150+i*60, 450, 60, 20});
}

void Widget::fire(Tank &tank, int owner)
{
    if (!tank.fire) return;
    tank.fire = false;

    int dx = 0, dy = 0;
    if (tank.dir == 0) dy = -10;
    if (tank.dir == 1) dx = 10;
    if (tank.dir == 2) dy = 10;
    if (tank.dir == 3) dx = -10;

    bullets.append({tank.x + 20, tank.y + 20, tank.dir, owner});
}

void Widget::spawnEnemy()
{
    if (currentMode == SINGLE && rand() % 120 == 0) {
        enemies.append({rand()%700+50, 50, 2, 40, 40, 3, false});
    }
}

void Widget::updateGame()
{
    if (gameOver) return;

    // 玩家1移动（↑↓←→）
    if (p1_up)    { player1.dir = 0; player1.y -= player1.speed; }
    if (p1_down)  { player1.dir = 2; player1.y += player1.speed; }
    if (p1_left)  { player1.dir = 3; player1.x -= player1.speed; }
    if (p1_right) { player1.dir = 1; player1.x += player1.speed; }
    fire(player1, 0);

    // 玩家2移动（WASD）
    if (currentMode == TWO_PLAYER) {
        if (p2_w) { player2.dir = 0; player2.y -= player2.speed; }
        if (p2_s) { player2.dir = 2; player2.y += player2.speed; }
        if (p2_a) { player2.dir = 3; player2.x -= player2.speed; }
        if (p2_d) { player2.dir = 1; player2.x += player2.speed; }
        fire(player2, 1);
    }

    // ✅ 这里是新增的子弹移动逻辑
    for (int i = 0; i < bullets.size(); i++) {
        auto &b = bullets[i];
        if (b.dir == 0) b.y -= 10;
        if (b.dir == 1) b.x += 10;
        if (b.dir == 2) b.y += 10;
        if (b.dir == 3) b.x -= 10;

        if (b.x < 0 || b.x > width() || b.y < 0 || b.y > height()) {
            bullets.removeAt(i);
        }
    }

    // 敌人移动 & 开火
    for (int i=0; i<enemies.size(); i++) {
        auto &e = enemies[i];
        e.y += e.speed;
        if (e.y > height()) { enemies.removeAt(i); continue; }
        if (rand() % 80 == 0) {
            bullets.append({e.x+20, e.y+20, e.dir, 2});
        }
    }

    spawnEnemy();
    checkCollisions();
    update();
}
void Widget::checkCollisions()
{
    // 子弹撞墙
    for (int i=0; i<bullets.size(); i++) {
        auto &b = bullets[i];
        for (int j=0; j<walls.size(); j++) {
            auto &w = walls[j];
            if (b.x > w.x && b.x < w.x+w.w && b.y > w.y && b.y < w.y+w.h) {
                bullets.removeAt(i);
                walls.removeAt(j);
                break;
            }
        }
    }

    // 单人模式：玩家撞敌人/敌人子弹
    if (currentMode == SINGLE) {
        for (auto &e : enemies) {
            if (abs(player1.x - e.x) < 40 && abs(player1.y - e.y) < 40) {
                gameOver = true;
                winner = 2;
            }
        }
        for (auto &b : bullets) {
            if (b.owner == 2 && abs(b.x - player1.x) < 25 && abs(b.y - player1.y) < 25) {
                gameOver = true;
                winner = 2;
            }
        }
    }

    // 双人模式：互相攻击
    if (currentMode == TWO_PLAYER) {
        for (auto &b : bullets) {
            if (b.owner == 0 && abs(b.x - player2.x) < 25 && abs(b.y - player2.y) < 25) {
                gameOver = true;
                winner = 0;
            }
            if (b.owner == 1 && abs(b.x - player1.x) < 25 && abs(b.y - player1.y) < 25) {
                gameOver = true;
                winner = 1;
            }
        }
    }

    // 边界限制
    if (player1.x < 0) player1.x = 0;
    if (player1.x > width()-40) player1.x = width()-40;
    if (player1.y < 0) player1.y = 0;
    if (player1.y > height()-40) player1.y = height()-40;

    if (currentMode == TWO_PLAYER) {
        if (player2.x < 0) player2.x = 0;
        if (player2.x > width()-40) player2.x = width()-40;
        if (player2.y < 0) player2.y = 0;
        if (player2.y > height()-40) player2.y = height()-40;
    }
}

void Widget::drawTank(QPainter &p, Tank &tank, QColor color)
{
    p.setBrush(color);
    p.setPen(Qt::black);
    p.drawRect(tank.x, tank.y, 40, 40);

    if (tank.dir == 0) p.drawRect(tank.x+15, tank.y-10, 10, 20);
    if (tank.dir == 1) p.drawRect(tank.x+40, tank.y+15, 20, 10);
    if (tank.dir == 2) p.drawRect(tank.x+15, tank.y+40, 10, 20);
    if (tank.dir == 3) p.drawRect(tank.x-10, tank.y+15, 20, 10);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(20,20,20));

    // 墙壁
    p.setBrush(QColor(100,100,100));
    for (auto &w : walls) p.drawRect(w.x, w.y, w.w, w.h);

    // 玩家1（绿色）
    drawTank(p, player1, QColor(0,200,0));

    // 玩家2（蓝色，双人模式）
    if (currentMode == TWO_PLAYER) {
        drawTank(p, player2, QColor(0,100,255));
    }

    // 敌人（红色）
    p.setBrush(QColor(200,0,0));
    for (auto &e : enemies) drawTank(p, e, QColor(200,0,0));

    // 子弹
    p.setBrush(Qt::yellow);
    for (auto &b : bullets) p.drawEllipse(b.x, b.y, 6, 6);

    // 模式提示
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 16));
    if (currentMode == SINGLE) p.drawText(20, 30, "模式：单人对战");
    if (currentMode == TWO_PLAYER) p.drawText(20, 30, "模式：双人对战");

    // 游戏结束
    if (gameOver) {
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 40, QFont::Bold));
        if (currentMode == SINGLE) {
            p.drawText(rect(), Qt::AlignCenter, "GAME OVER");
        } else {
            if (winner == 0) p.drawText(rect(), Qt::AlignCenter, "绿色玩家胜利！");
            if (winner == 1) p.drawText(rect(), Qt::AlignCenter, "蓝色玩家胜利！");
        }
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    // 玩家1控制
    if (e->key() == Qt::Key_Up)    p1_up = true;
    if (e->key() == Qt::Key_Down)  p1_down = true;
    if (e->key() == Qt::Key_Left)  p1_left = true;
    if (e->key() == Qt::Key_Right) p1_right = true;
    if (e->key() == Qt::Key_Space) player1.fire = true;

    // 玩家2控制（WASD + 左Ctrl）
    if (currentMode == TWO_PLAYER) {
        if (e->key() == Qt::Key_W) p2_w = true;
        if (e->key() == Qt::Key_S) p2_s = true;
        if (e->key() == Qt::Key_A) p2_a = true;
        if (e->key() == Qt::Key_D) p2_d = true;
        if (e->key() == Qt::Key_Control) player2.fire = true;
    }

    // 切换模式
    if (e->key() == Qt::Key_1) initGame(SINGLE);
    if (e->key() == Qt::Key_2) initGame(TWO_PLAYER);
    if (e->key() == Qt::Key_R) initGame(currentMode);
}

void Widget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Up)    p1_up = false;
    if (e->key() == Qt::Key_Down)  p1_down = false;
    if (e->key() == Qt::Key_Left)  p1_left = false;
    if (e->key() == Qt::Key_Right) p1_right = false;

    if (currentMode == TWO_PLAYER) {
        if (e->key() == Qt::Key_W) p2_w = false;
        if (e->key() == Qt::Key_S) p2_s = false;
        if (e->key() == Qt::Key_A) p2_a = false;
        if (e->key() == Qt::Key_D) p2_d = false;
    }
}