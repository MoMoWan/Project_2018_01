
cd Bin
del Xanova_XM380_Mouse_FW_v0.00.00_*.hex

set Ymd=%date:~0,4%%date:~5,2%%date:~8,2%

copy ..\Objects\Galaxy_Mouse.hex Xanova_XM380_Mouse_FW_v0.00.00_%Ymd%.hex


