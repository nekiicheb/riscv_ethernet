# riscv_ethernet
test task from company

## Задание
![Task](./tcompany_task.png)

## todo list:
    - уточнить какой полином в crc32. Сейчас в коде проверяется контрольная сумма IPv4 Header, а нужно crc всей посылки
    - запуск эмулятора qemu polarfire
    - убедиться в работе цепочки дескриптора
    - убедиться в нормальной работе цепочки дескрипторов при переполнении 
    - убедиться в отсутствии исключения при доступе к невыровненным адресам (для CortexA9 + MPU актуально)
    - добавить в проект тестирования под Linux приём пакетов и чтение из консоли

## Решение
1) проект решение задания для платформы polarfire для SOFTCONSOLE IDE
	./softconsole_wb/mpfs-mac-simple-test/
    - Исходник проекта ./softconsole_wb/mpfs-mac-simple-test/src/application/hart0/	
    -  если требуется включить UNIT TEST, то включаем #define CHECKSUM_TEST в ./softconsole_wb/mpfs-mac-simple-test/src/application/hart0/project_cfg.h 
2) проект для тестирования решения под linux. 
    - исходники ./eclipse-workspace/client_raw/src/
    - запуск: Открываем raw socket и отправляем в localhost пакет с корректной контрольной суммой IP заголовка, затем пакет с некорректной контрольной суммой IP заголовка.
    - sudo ./eclipse-workspace/client_raw/src/client_raw Запускаем под sudo, иначе raw socket не создается.
3) скрипт для запуска QEMU (не работает)
    - ./sftcnsle_scripts/qemu_run.sh
    - проброс портов в QEMU машину, dump трафика для wireshark
4) проект для эмуляции кода в QEMU для FreedomStudio IDE
    - ./freedom_wb/qemu_sifive_e31_sifive_welcome/src/

