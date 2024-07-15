#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

// 枚举变量类型
enum class ValueType {
    INTEGER,
    FLOAT,
    STRING
};

// 变量值的结构
struct Value {
    ValueType type;
    union {
        int intValue;
        float floatValue;
    };
    std::string stringValue;

    // 构造函数
    Value() : type(ValueType::INTEGER), intValue(0) {}
    Value(int value) : type(ValueType::INTEGER), intValue(value) {}
    Value(float value) : type(ValueType::FLOAT), floatValue(value) {}
    Value(const std::string& value) : type(ValueType::STRING), stringValue(value) {}

    // 获取字符串表示
    std::string toString() const {
        switch (type) {
            case ValueType::INTEGER:
                return std::to_string(intValue);
            case ValueType::FLOAT:
                return std::to_string(floatValue);
            case ValueType::STRING:
                return stringValue;
            default:
                return "Unknown";
        }
    }
};

// Drawer类使用Windows API创建窗口、按钮和标签
class Drawer {
public:
    Drawer() : hWnd(nullptr) {}

    void createWindow(const std::string& title, int width, int height) {
        const char* className = "IngotScriptWindowClass";

        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = className;

        RegisterClass(&wc);

        hWnd = CreateWindowEx(
            0,
            className,
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        if (hWnd == NULL) {
            std::cerr << "Error: Unable to create window!" << std::endl;
            return;
        }

        ShowWindow(hWnd, SW_SHOW);
    }

    void createButton(const std::string& text, int x, int y, int w, int h) {
        CreateWindow(
            "BUTTON",
            text.c_str(),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            x, y, w, h,
            hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );
    }

    void createLabel(const std::string& text, int x, int y, int w, int h) {
        CreateWindow(
            "STATIC",
            text.c_str(),
            WS_VISIBLE | WS_CHILD,
            x, y, w, h,
            hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );
    }

    void run() {
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    HWND hWnd;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
};

// 解释器类
class Interpreter {
private:
    std::unordered_map<std::string, Value> variables;
    Drawer drawer;

public:
    void execute(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            processLine(line);
        }
        file.close();
    }

    void run() {
        drawer.run();
    }

private:
    void processLine(const std::string& line) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "let") {
            std::string varName, eq;
            iss >> varName >> eq;

            if (eq == "=") {
                Value value = evaluateExpression(iss);
                variables[varName] = value;
            }
        } else if (command == "print") {
            std::string token;
            iss >> std::ws;
            std::getline(iss, token);

            if (!token.empty()) {
                if (token[0] == '"' && token.back() == '"') {
                    std::string strValue = token.substr(1, token.size() - 2); // 去掉引号
                    std::cout << strValue << std::endl;
                } else if (isNumeric(token)) {
                    std::cout << token << std::endl;
                } else if (variables.find(token) != variables.end()) {
                    std::cout << variables[token].toString() << std::endl;
                } else {
                    std::cerr << "Error: Invalid print statement" << std::endl;
                }
            } else {
                std::cerr << "Error: Invalid print statement" << std::endl;
            }
        } else if (command == "if") {
            std::string condition;
            std::getline(iss >> std::ws, condition);
            bool result = evaluateCondition(condition);
            if (result) {
                processBlock(iss);
            }
        } else if (command == "while") {
            std::string condition;
            std::getline(iss >> std::ws, condition);
            while (evaluateCondition(condition)) {
                processBlock(iss);
            }
        } else if (command == "for") {
            std::string varName, eq, start, toKeyword, end;
            iss >> varName >> eq >> start >> toKeyword >> end;
            if (eq != "=" || toKeyword != "to") {
                std::cerr << "Error: Invalid for loop syntax" << std::endl;
                return;
            }
            int startValue = std::stoi(start);
            int endValue = std::stoi(end);
            for (int i = startValue; i <= endValue; ++i) {
                variables[varName] = Value(i);
                processBlock(iss);
            }
        } else if (command == "Drawer") {
            std::string funcName;
            iss >> funcName;
            if (funcName == "createWindow") {
                drawer.createWindow("Drawer Window", 800, 600);
            } else if (funcName == "createButton") {
                std::string buttonText;
                int x, y, w, h;
                iss >> std::quoted(buttonText) >> x >> y >> w >> h;
                drawer.createButton(buttonText, x, y, w, h);
            } else if (funcName == "createLabel") {
                std::string labelText;
                int x, y, w, h;
                iss >> std::quoted(labelText) >> x >> y >> w >> h;
                drawer.createLabel(labelText, x, y, w, h);
            } else {
                std::cerr << "Error: Unknown Drawer function: " << funcName << std::endl;
            }
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
        }
    }

    void processBlock(std::istringstream& iss) {
        std::string line;
        std::string block;
        while (std::getline(iss, line)) {
            if (line == "end") {
                break;
            }
            block += line + "\n";
        }
        executeBlock(block);
    }

    void executeBlock(const std::string& block) {
        std::istringstream iss(block);
        std::string line;
        while (std::getline(iss, line)) {
            processLine(line);
        }
    }

    Value evaluateExpression(std::istringstream& iss) {
        std::string token;
        iss >> token;
        if (isNumeric(token)) {
            if (token.find('.') != std::string::npos) {
                float value = std::stof(token);
                return Value(value);
            } else {
                int value = std::stoi(token);
                return Value(value);
            }
        } else if (token[0] == '"') {
            std::string strValue = token.substr(1);
            while (token.back() != '"') {
                iss >> token;
                strValue += " " + token;
            }
            strValue.pop_back();
            return Value(strValue);
        } else if (variables.find(token) != variables.end()) {
            return variables[token];
        } else {
            throw std::invalid_argument("Unknown token: " + token);
        }
    }

    bool evaluateCondition(const std::string& condition) {
        std::istringstream iss(condition);
        int left, right;
        std::string op;
        iss >> left >> op >> right;

        if (op == "==") {
            return left == right;
        } else if (op == "!=") {
            return left != right;
        } else if (op == "<") {
            return left < right;
        } else if (op == ">") {
            return left > right;
        } else if (op == "<=") {
            return left;
        } else if (op == ">=") {
            return left >= right;
        } else {
            std::cerr << "Error: Invalid comparison operator" << std::endl;
            return false;
        }
    }

    bool isNumeric(const std::string& str) {
        return !str.empty() && std::find_if(str.begin(), str.end(), [](char c) { return !std::isdigit(c) && c != '.'; }) == str.end();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <script.is>" << std::endl;
        return 1;
    }

    Interpreter interpreter;
    interpreter.execute(argv[1]);
    interpreter.run();
    return 0;
}
