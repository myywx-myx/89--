#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(R"(
    /* 全局字体：跨系统统一，看着舒服 */
    QWidget {
        font-family: "Microsoft YaHei", "PingFang SC", sans-serif;
        font-size: 14px;
        color: #333333;
    }

    /* 所有按钮基础样式：圆角、间距、 hover 反馈 */
    QPushButton {
        padding: 6px 16px;
        border-radius: 4px;
        border: none;
        background-color: #f0f0f0;
    }
    QPushButton:hover { background-color: #e0e0e0; }
    QPushButton:pressed { background-color: #d0d0d0; }

    /* 自动识别「添加」类按钮：蓝色主色调 */
    QPushButton[text*="添加"] {
        background-color: #409eff;
        color: white;
    }
    QPushButton[text*="添加"]:hover { background-color: #66b1ff; }

    /* 自动识别「删除」类按钮：浅红色，区分主次 */
    QPushButton[text*="删除"] {
        background-color: #f56c6c;
        color: white;
    }
    QPushButton[text*="删除"]:hover { background-color: #f78989; }

    /* 输入框/文本域样式：圆角、聚焦高亮 */
    QTextEdit, QLineEdit {
        border: 1px solid #e9ecef;
        border-radius: 4px;
        padding: 8px;
    }
    QTextEdit:focus, QLineEdit:focus {
        border-color: #409eff;
    }

    /* 列表样式：交替行颜色、选中高亮 */
    QListWidget {
        border: 1px solid #e9ecef;
        border-radius: 4px;
        padding: 4px;
    }
    QListWidget::item {
        padding: 8px;
        border-bottom: 1px solid #f5f5f5;
    }
    QListWidget::item:selected {
        background-color: #ecf5ff;
        color: #409eff;
    }
    QListWidget::item:alternate {
        background-color: #fafafa;
    }
)");
    MainWindow w;
    w.setStyleSheet(R"(
    MainWindow {
        /* 从左上到右下淡蓝→浅灰的渐变，不用找图片，Qt自带 */
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 #f5f7fa, stop:1 #c3cfe2);
    }
    /* 所有输入框/按钮/列表加半透明白色底，字不会和背景糊在一起 */
    QTextEdit, QLabel, QListWidget, QPushButton {
        background-color: rgba(255, 255, 255, 0.85);
        border-radius: 4px;
    }
)");
    w.show();
    return a.exec();
}