# TaMP — Подгруппа 3

Клиент-серверное приложение на C++/Qt для авторизации пользователей и интерактивной визуализации кусочно-заданной математической функции.

---

## Структура проекта

```
TaMP/
├── TaMP_Client/       # GUI-клиент (Qt Widgets)
├── TaMP_Server/       # TCP-сервер (Qt Network + SQLite)
├── docker/            # Docker-файлы для сервера
├── docs/              # Автодокументация Doxygen (HTML)
└── Doxyfile           # Конфигурация Doxygen
```

---

## Возможности

### Сервер
- TCP-сервер на базе `QTcpServer` (порт 12345)
- Хранение пользователей в SQLite
- Авторизация, регистрация, восстановление пароля через email (Mail.ru SMTP/SSL)
- Кастомный текстовый протокол с разделителем `&`

### Клиент
- Окно авторизации и регистрации
- Трёхшаговый мастер восстановления пароля
- Интерактивный график кусочной функции (QCustomPlot)
- Таблица значений f(x) с настройкой параметров через слайдеры
- Тема оформления Catppuccin Mocha

---

## Сборка

### Требования
- Qt 6.x
- CMake 3.16+
- C++17

### Сервер
```bash
cd TaMP_Server
cmake -B build
cmake --build build
```

### Клиент
```bash
cd TaMP_Client
cmake -B build
cmake --build build
```

---

## Запуск через Docker

```bash
# 1. Создать файл с учётными данными SMTP
cp TaMP_Server/email.txt.example TaMP_Server/email.txt
# Заполнить: email = your@mail.ru  и  key = app_password

# 2. Собрать образ
docker build -f docker/Dockerfile -t tamp-server .

# 3. Запустить
docker run -d --name tamp-server -p 12345:12345 tamp-server
```

> Если возникают DNS-ошибки при сборке на Linux — запустить `docker/fix-dns.sh`

---

## Протокол

| Запрос | Ответ (успех) | Ответ (ошибка) |
|--------|--------------|----------------|
| `auth&{login}&{md5}` | `auth+&{name}` | `auth-` |
| `reg&{login}&{name}&{md5}&{email}` | `reg+&{login}` | `reg-` |
| `reset&{login}` | `reset+&{maskedEmail}` | `reset-` |
| `confirm&{login}&{code}` | `confirm+` | `confirm-` |
| `newpass&{login}&{md5}` | `newpass+` | `newpass-` |

---

## Документация

Сгенерирована Doxygen, доступна в `docs/html/index.html`.

```bash
doxygen Doxyfile
```
