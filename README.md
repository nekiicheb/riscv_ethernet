# riscv_ethernet
test task from company


Тестовый проект
1) todo list:
    - запуск эмулятора qemu polarfire
      - убедиться в работе цепочки дескриптора
      - убедиться в нормальной работе цепочки дескрипторов при переполнении 
      - убедиться в отсутствии исключения при доступе к невыровненным адресам (для CortexA9 + MPU актуально)
      - добавить в проект тестирования под Linux приём пакетов и чтение из консоли
    - реализовать скрипты htonl и т д
2) структура проекта:
  2.1) проект решение задания для платформы polarfire для SOFTCONSOLE IDE
	./softconsole_wb/mpfs-mac-simple-test/
    2.1.1) Исходник проекта ./softconsole_wb/mpfs-mac-simple-test/src/application/hart0/	
    2.1.2) если требуется включить UNIT TEST, то включаем #define CHECKSUM_TEST в ./softconsole_wb/mpfs-mac-simple-test/src/application/hart0/project_cfg.h 
  2.2) проект для тестирования решения под linux. 
    2.2.1) исходники ./eclipse-workspace/client_raw/src/
    2.2.2) Описание: Открываем raw socket и отправляем в localhost пакет с корректной контрольной суммой IP заголовка, затем пакет с некорректной контрольной суммой IP заголовка.
    2.2.3) sudo ./eclipse-workspace/client_raw/src/client_raw Запускаем под sudo, иначе raw socket не создается.
  2.3) скрипт для запуска QEMU (не работает)
    2.3.1) ./sftcnsle_scripts/qemu_run.sh
    2.3.2) проброс портов в QEMU машину, dump трафика для wireshark

