import sys
from qt_hummingbird import HummingbirdWindow
from PyQt5.QtWidgets import QApplication

if __name__ == "__main__":
    app = QApplication(sys.argv)
    m = HummingbirdWindow()
    m.show()
    m.raise_()
    sys.exit(app.exec_())
