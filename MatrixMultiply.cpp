#include <iostream>
#include <vector>
#include <stdexcept>
#include <limits>
#include <sstream> 

class Matrix {
private:
    std::vector<std::vector<double>> data;
    size_t rows;
    size_t cols;

public:
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Размеры матрицы должны быть положительными числами");
        }
        data.resize(rows, std::vector<double>(cols, 0.0));
    }

    Matrix(size_t rows, size_t cols, const std::vector<std::vector<double>>& values) : rows(rows), cols(cols) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Размеры матрицы должны быть положительными числами");
        }
        if (values.size() != rows || (rows > 0 && values[0].size() != cols)) {
            throw std::invalid_argument("Несоответствие размеров при инициализации");
        }
        data = values;
    }

    size_t getRows() const {
        return rows;
    }

    size_t getCols() const {
        return cols;
    }

    double& operator()(size_t i, size_t j) {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Индекс выходит за границы матрицы");
        }
        return data[i][j];
    }

    const double& operator()(size_t i, size_t j) const {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Индекс выходит за границы матрицы");
        }
        return data[i][j];
    }

    void print() const {
        std::cout << "Матрица " << rows << "x" << cols << ":\n";
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << data[i][j] << "\t";
            }
            std::cout << "\n";
        }
    }

    static std::string intToString(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static std::string sizeToString(size_t value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static Matrix multiply(const Matrix& A, const Matrix& B) {
        if (A.getCols() != B.getRows()) {
            std::string errorMsg = "Количество столбцов первой матрицы (";
            errorMsg += sizeToString(A.getCols());
            errorMsg += ") должно равняться количеству строк второй матрицы (";
            errorMsg += sizeToString(B.getRows());
            errorMsg += ")";
            throw std::invalid_argument(errorMsg);
        }

        if (A.getRows() > std::numeric_limits<size_t>::max() / B.getCols()) {
            throw std::overflow_error("Возможное переполнение при создании результирующей матрицы");
        }

        size_t m = A.getRows();
        size_t n = A.getCols();
        size_t p = B.getCols();

        Matrix result(m, p);

        try {
            for (size_t i = 0; i < m; ++i) {
                for (size_t j = 0; j < p; ++j) {
                    double sum = 0.0;
                    for (size_t k = 0; k < n; ++k) {
                        double product = A(i, k) * B(k, j);
                        if ((A(i, k) != 0.0 && product / A(i, k) != B(k, j)) ||
                            (B(k, j) != 0.0 && product / B(k, j) != A(i, k))) {
                            throw std::overflow_error("Переполнение при умножении элементов");
                        }

                        double new_sum = sum + product;
                        if ((product > 0 && new_sum < sum) || (product < 0 && new_sum > sum)) {
                            throw std::overflow_error("Переполнение при сложении элементов");
                        }
                        sum = new_sum;
                    }
                    result(i, j) = sum;
                }
            }
        }
        catch (const std::exception& e) {
            std::string errorMsg = "Ошибка при умножении матриц: ";
            errorMsg += e.what();
            throw std::runtime_error(errorMsg);
        }

        return result;
    }

    Matrix operator*(const Matrix& other) const {
        return multiply(*this, other);
    }
};

template<typename T>
T safeInput(const std::string& prompt, T minValue = std::numeric_limits<T>::min(),
    T maxValue = std::numeric_limits<T>::max()) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка: введите корректное число.\n";
        }
        else if (value < minValue || value > maxValue) {
            std::cout << "Ошибка: число должно быть в диапазоне от "
                << minValue << " до " << maxValue << ".\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

Matrix inputMatrix(const std::string& name) {
    std::cout << "\n=== Ввод матрицы " << name << " ===\n";

    int rows = safeInput<int>("Введите количество строк: ", 1, 100);
    int cols = safeInput<int>("Введите количество столбцов: ", 1, 100);

    Matrix matrix(rows, cols);

    std::cout << "Введите элементы матрицы построчно:\n";
    for (int i = 0; i < rows; ++i) {
        std::cout << "Строка " << (i + 1) << ": ";
        for (int j = 0; j < cols; ++j) {
            double value;
            std::cout << "Элемент [" << (i + 1) << "][" << (j + 1) << "]: ";
            value = safeInput<double>("", -1e100, 1e100);
            matrix(i, j) = value;
        }
    }

    return matrix;
}

void demonstrateExample() {
    std::cout << "\n=== Пример умножения матриц ===\n";

    std::vector<std::vector<double>> valuesA;
    valuesA.resize(2);
    valuesA[0].resize(3);
    valuesA[1].resize(3);

    valuesA[0][0] = 1; valuesA[0][1] = 2; valuesA[0][2] = 3;
    valuesA[1][0] = 4; valuesA[1][1] = 5; valuesA[1][2] = 6;

    std::vector<std::vector<double>> valuesB;
    valuesB.resize(3);
    valuesB[0].resize(2);
    valuesB[1].resize(2);
    valuesB[2].resize(2);

    valuesB[0][0] = 7;  valuesB[0][1] = 8;
    valuesB[1][0] = 9;  valuesB[1][1] = 10;
    valuesB[2][0] = 11; valuesB[2][1] = 12;

    try {
        Matrix A(2, 3, valuesA);
        Matrix B(3, 2, valuesB);

        std::cout << "Матрица A:\n";
        A.print();

        std::cout << "\nМатрица B:\n";
        B.print();

        Matrix C = Matrix::multiply(A, B);

        std::cout << "\nРезультат умножения A * B:\n";
        C.print();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

int main() {
    setlocale(LC_ALL, "Rus");
    std::cout << "Программа умножения матриц\n";

    char choice;
    std::cout << "Хотите использовать пример (e) или ввести матрицы вручную (m)? [e/m]: ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == 'e' || choice == 'E') {
        demonstrateExample();
    }
    else {
        try {
            Matrix A = inputMatrix("A");
            Matrix B = inputMatrix("B");

            std::cout << "\n=== Введенные матрицы ===\n";
            std::cout << "Матрица A:\n";
            A.print();

            std::cout << "\nМатрица B:\n";
            B.print();

            std::cout << "\n=== Умножение матриц ===\n";
            Matrix C = Matrix::multiply(A, B);

            std::cout << "Результат умножения A * B:\n";
            C.print();

        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка аргумента: " << e.what() << std::endl;
            return 1;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Ошибка диапазона: " << e.what() << std::endl;
            return 1;
        }
        catch (const std::overflow_error& e) {
            std::cerr << "Ошибка переполнения: " << e.what() << std::endl;
            return 1;
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Ошибка выполнения: " << e.what() << std::endl;
            return 1;
        }
        catch (const std::exception& e) {
            std::cerr << "Неизвестная ошибка: " << e.what() << std::endl;
            return 1;
        }
    }

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.get();

    return 0;
}