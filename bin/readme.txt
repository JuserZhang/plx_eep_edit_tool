************把该文件夹下的 eep_edit_tool 复制到/usr/bin/ 目录下***********

【使用方法】
————————————————————————————————
运行：eep_edit_tool   *.bin
例如：eep_edit_tool   07020046_PLX8717_ROM_HRCW_PCIE3_V102.bin
————————————————————————————————
【命令解释】
————————————————————————————————
dl	   显示eeprom文件的原始数据
dl [offset]    显示eeprom文件偏移地址为offset的数据，以dword为步长偏移
el [offset]    修改eeprom文件在offset偏移的数据，以dword为步长偏移
modver       修改eeprom的版本信息以及序列号
version        查看eeprom的版本相关信息
help            帮助
quit            退出程序
————————————————————————————————

作者：宏杉科技 张鹏<zhangpeng@macrosan.com>