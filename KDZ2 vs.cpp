#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <set>

#define SUB_SYM 4

struct Counter {
    int counter;
};

class Algorithms {
public:
    std::set<std::vector<int>> errors;

    int64_t doTests(const std::string& base_string, std::string& substr, int type_) {
        auto start = std::chrono::high_resolution_clock::now();
        run(base_string, substr, type_);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        int64_t nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
            elapsed).count();
        return nanoseconds;
    }

private:
    std::vector<int> ideal;

    void run(const std::string& base_string, std::string& substr, int type_) {
        std::vector<int> curr;
        switch (type_) {
        case 0:
            ideal = standardComparison(base_string, substr);
            break;
        case 1:
            curr = kMPWithStandardBorders(base_string, substr);
            if (ideal != curr) {
                //std::cout << "Error! ";
                //errors.insert(curr);
            }
            break;
        case 2:
            curr = kMPWithVerBorders(base_string, substr);
            if (ideal != curr) {
                //std::cout << "Error ";
                //std::cout << base_string << " " << substr << std::endl;
            }
            break;
        case 3:
            //curr = mergeKMP(base_string, substr);
            //if (ideal != curr) {
                //std::cout << "Error ";
                //std::cout << base_string << " " << substr << std::endl;
            //}
            break;
        }
    }
    /// <summary>
    /// Наивный алгоритм
    /// </summary>
    /// <param name="base_string">Базовая строка</param>
    /// <param name="substr">Подстрока</param>
    /// <returns>Все вхождения подстроки в строку</returns>
    std::vector<int> standardComparison(const std::string& base_string, const std::string& substr) {
        bool flag = true;
        std::vector<int> entries;
        for (size_t i = 0; i <= base_string.size() - substr.size(); i++) {
            for (size_t j = 0; j < substr.size(); j++) {
                if (substr[j] == '?') {
                    continue;
                }
                if (base_string[i + j] != substr[j]) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                entries.push_back(i);
            }
            flag = true;
        }
        if (entries.empty()) {
            entries.push_back(-1);
        }
        return entries;
    }

    bool compEdge(std::string& str, size_t& last, size_t& k) {
        bool res = true;
        for (size_t i = 0; i <= k; ++i) {
            if (!checkSymbs(str[i], str[last - k + i])) {
                res = false;
                break;
            }
        }
        return res;
    }

    /// <summary>
    /// Проверка символов на равенство или на знак вопроса
    /// </summary>
    /// <param name="symb1">Первый символ</param>
    /// <param name="symb2">Второй символ</param>
    /// <returns>Равны ли символы или является ли хотя бы один из них заком вопроса</returns>
    bool checkSymbs(const char& symb1, const char& symb2) {
        return (symb1 == symb2 || symb2 == '?' || symb1 == '?');
    }

    /// <summary>
    /// Префиксная функция
    /// </summary>
    /// <param name="substr">Подстрока</param>
    /// <returns>Массив префиксов</returns>
    std::vector<int> calcPref(std::string& substr) {
        std::vector<int> br(substr.size());
        br[0] = 0;

        int j = 0;
        for (size_t i = 1; i < substr.size(); i++, j = br[i - 1]) {
            bool flag = false;
            if (substr[j] == '?') {
                flag = true;
            }
            if (!flag) {
                while (j > 0 && !checkSymbs(substr[j], substr[i])) {
                    j = br[j - 1];
                    if (substr[j] == '?') {
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    if (checkSymbs(substr[j], substr[i])) {
                        j++;
                    }
                    br[i] = j;
                }
            }
            if (flag) {
                br[i] = 0;
                for (size_t k = 0; k < i; ++k) {
                    if (compEdge(substr, i, k)) {
                        br[i] = k + 1;
                    }
                }
            }
        }
        return br;
    }

    /// <summary>
    /// КМП со стандартными гранями
    /// </summary>
    /// <param name="base_string">Базовая строка</param>
    /// <param name="substr">Подстрока</param>
    /// <returns>Все вхождения подстроки в строку</returns>
    std::vector<int> kMPWithStandardBorders(const std::string& base_string, std::string& substr) {
        int n = base_string.size();
        int m = substr.size();
        std::vector<int> pi = calcPref(substr);
        std::vector<int> entries;
        int i = 0;
        int kol = 0;  // число совпавших символов
        for (size_t i = 0; i < base_string.size();) {
            if (substr[kol] == base_string[i] || substr[kol] == '?') {
                ++i;
                ++kol;
            }
            else {
                if (kol != 0) {
                    kol = pi[kol - 1];
                }
                else {
                    ++i;
                }
            }
            if (kol == substr.size()) {
                entries.emplace_back(i - kol);
                kol = pi[kol - 1];
            }
        }
        if (entries.empty()) {
            entries.push_back(-1);
        }
        return entries;
    }

    /// <summary>
    /// КМП с уточнёнными гранями
    /// </summary>
    /// <param name="base_string">Базовая строка</param>
    /// <param name="substr">Подстрока</param>
    /// <returns>Все вхождения подстроки в строку</returns>
    std::vector<int> kMPWithVerBorders(const std::string& base_string, std::string& substr) {
        int n = base_string.size();
        int m = substr.size();
        std::vector<int> pi = calcPref(substr);
        std::vector<size_t> pref_t(substr.size());
        pref_t[0] = 0;
        for (size_t i = 1; i < substr.size() - 1; ++i) {
            if (!checkSymbs(substr[pi[i]], substr[i + 1])) {
                pref_t[i] = pi[i];
            }
            else {
                pref_t[i] = pref_t[pi[i - 1]];
            }
        }
        pref_t[substr.size() - 1] = pi[substr.size() - 1];
        std::vector<int> entries;
        int i = 0;
        int kol = 0;  // число совпавших символов
        for (size_t i = 0; i < base_string.size();) {
            if (substr[kol] == base_string[i] || substr[kol] == '?') {
                ++i;
                ++kol;
            }
            else {
                if (kol != 0) {
                    kol = pref_t[kol - 1];
                }
                else {
                    ++i;
                }
            }
            if (kol == substr.size()) {
                entries.emplace_back(i - kol);
                kol = pref_t[kol - 1];
            }
        }
        if (entries.empty()) {
            entries.push_back(-1);
        }
        return entries;
    }

    /// <summary>
    /// Взято отсюда: https://habr.com/ru/articles/307220/
    /// </summary>
    /// <param name="base_string">Базовая строка</param>
    /// <param name="substr">Подстрока</param>
    /// <returns>Все вхождения подстроки в строку</returns>
    std::vector<int> mergeKMP(const std::string& base_string, std::string& substr) {
        std::string m_string = substr + "@" + base_string;
        std::vector<int> entries;
        auto pi = calcPref(m_string);
        for (size_t i = 0; i < pi.size(); i++) {
            if (pi[i] = substr.size()) {
                entries.push_back(i - 2 * substr.size());
            }
        }
        if (entries.empty()) {
            entries.push_back(-1);
        }
        return entries;
    }
};

class Generator {
public:
    Generator() {
        srand(time(nullptr));
    }

    std::string generateBaseString(int type, int len) {
        return type ? generateFourSymbolsTemplate(len) : generateBinaryTemplate(len);
    }

private:
    std::string generateBinaryTemplate(int len) {
        std::string res;
        for (int i = 0; i < len; ++i) {
            res += std::to_string(rand() % 2);
        }
        return res;
    }

    std::string generateFourSymbolsTemplate(int len) {
        std::string res;
        for (int i = 0; i < len; ++i) {
            res += 'a' + static_cast<char>(rand() % 4);
        }
        return res;
    }
};

/// <summary>
/// Функция добавления SUB_SYM знаков вопроса в шаблон
/// </summary>
/// <param name="substr"></param>
/// <returns>Шаблон со знаками вопроса</returns>
std::string changeTemplate(std::string substr) {
    srand(time(nullptr));
    for (int i = 0; i < SUB_SYM; ++i) {
        size_t pos = 0;
        do {
            pos = rand() % substr.size();
            // Дважды заменить один символ не выйдет
        } while (substr[pos] == '?');
        // Изменение шаблона происходит путём замены нескольких символов на знак вопроса
        substr[pos] = '?';
    }
    return substr;
}

/// <summary>
/// Сама тестирующая функция
/// </summary>
void test() {
    std::string base;
    Generator generator;
    std::ofstream out;
    out.open("results4.csv", std::ios_base::app);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            int len = j ? 100000 : 10000;
            base = generator.generateBaseString(i, len);
            Algorithms algorithm;
            for (int l = 100; l <= 3000; l += 100) {
                int64_t res[4] = { 0, 0, 0, 0};
                std::string substr;
                for (int k = 0; k < 10; ++k) {
                    int pos = rand() % (len - l);
                    substr = base.substr(pos, l);
                    substr = changeTemplate(substr);
                    for (int m = 0; m < 4; ++m) {
                        res[m] += algorithm.doTests(base, substr, m)/10;
                    }
                }
                std::string arr_type = i ? "Four-symbol" : "binary";
                out << "Array type: " << arr_type << ";Length: " << len << ";Template length: " << l;
                std::cout << "Array type: " << arr_type << ";Length: " << len << ";Template length: " << l << ";Result: " << res[0] << std::endl;
                // Приходится делить на 1000, потому что иначе мой эксель не может нормально обработать эти данные
                for (const double& a : res) {
                    out << ";" << std::round(a/1000);
                }
                out << std::endl;
            }
        }
    }
}

int main() {
    std::cout << "Started" << std::endl;
    srand(time(nullptr));
    test();
    std::cout << "Finished";

    //std::string a = "";
    //std::string b = "100?";
    //Algorithms alg;
    //std::cout << alg.kMPWithVerBorders(a, b)[0];
}
