Клиент может посылать серверу команды Get и Set. В потоковой функции в программе сервера происходит сначала прием команды от клиента, потом - выполнение действий в соответствии с данной командой.
В одной потоковой функции происходит обработка одного запроса от клиента. Затем соединение закрывается. Для выполнения нового запроса происходит новое подключение и вызывается очередная потоковая функция.

На каждого клиента предусмотрена одна запись в БД. Ключом к записи служит ip клиента (правильно ли это?)
