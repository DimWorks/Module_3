Задача 2.5: Написать программу, выводящую содержимое указаного каталога с указание типа файлов.

Для решения данной задачи использовались функции:

1. lstat() - 	Предназначена для получения статуса файла (например "S_IFDIR" говорит о том, что перед нами директория (каталог)
2. opendir() - 	Предназначена для открытия каталога по указанному пути
3. readdir() - 	Предназначена для чтения содержимого каталога
4. closedir() - Предназначеня для закрытия каталога после работы с ним, чтобы в будущем освободить систему от этой операции
