说明：该工具仿真cli，可以像使用在线eeprom一样对eeprom文件进行离线编辑。

【使用方法】
————————————————————————————————
1.  make进行编译
2.  将要修改的eeprom文件放到与该工具相同目录，
3.  运行：./eep_edit_tool *.bin
例如：./eep_edit_tool 07020046_PLX8717_ROM_HRCW_PCIE3_V102.bin

为方便使用请把eep_edit_tool 复制到/user/bin
使用时只要：eep_edit_tool 需要修改的文件或路径
————————————————————————————————

【命令解释】
————————————————————————————————
dl	   显示eeprom文件的原始数据
dl [offset]    显示eeprom文件偏移地址为offset的数据，以dword为步长偏移
el [offset]    修改eeprom文件在offset偏移的数据，以dword为步长偏移
modver       修改eeprom的版本信息以及序列号
version        查看eeprom的版本相关信息
support       显示目前支持的版本信息和序列号
help            帮助
quit            退出程序
————————————————————————————————

【友情提示】
如果无法写入版本信息，显示serial_number is illegal!或者eeprom version is illegal!，需要把最新的序列号和版本信息添加到eepver.c 文件中的eep_serial_number_info和eep_version_info表中，然后再次进行编译使用。

【批量管理】
scripts目录下包含了一系列shell脚本，方便快速对eeprom image文件进行批量操作。

作者：宏杉科技 张鹏<zhangpeng@macrosan.com>