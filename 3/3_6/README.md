Задание 3.6: Изменить программу 2.7 так, чтобы дочерний процесс выводил информацию из файла. Если родительский процесс собирается записать в файл, то он посылает сигнал SIGUSR1. Когда родительский завершил модификацию файда, он посылает сигнал SIGUSR2. Дочерний процесс отправляет новое число родителю после разрешения.


