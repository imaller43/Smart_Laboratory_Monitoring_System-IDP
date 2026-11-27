<?xml version="1.0" encoding="utf-8"?>
<eagle version="7.7.0">
<drawing>
<settings><setting alwaysvectorfont="no"/><setting verticaltext="up"/></settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="yes" active="yes"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="yes" active="yes"/>
<layer number="17" name="Pads" color="2" fill="1" visible="yes" active="yes"/>
<layer number="18" name="Vias" color="2" fill="1" visible="yes" active="yes"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="yes" active="yes"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="yes" active="yes"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="yes" active="yes"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="yes" active="yes"/>
<layer number="25" name="tNames" color="7" fill="1" visible="yes" active="yes"/>
<layer number="26" name="bNames" color="7" fill="1" visible="yes" active="yes"/>
<layer number="27" name="tValues" color="7" fill="1" visible="yes" active="yes"/>
<layer number="28" name="bValues" color="7" fill="1" visible="yes" active="yes"/>
<layer number="45" name="Holes" color="7" fill="1" visible="yes" active="yes"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries><library name="IDP_Lab_Safety">
<packages>
<package name="HDR_1X22">
<wire x1="-1.35" y1="1.27" x2="1.35" y2="1.27" width="0.15" layer="21"/>
<wire x1="1.35" y1="1.27" x2="1.35" y2="-54.61" width="0.15" layer="21"/>
<wire x1="1.35" y1="-54.61" x2="-1.35" y2="-54.61" width="0.15" layer="21"/>
<wire x1="-1.35" y1="-54.61" x2="-1.35" y2="1.27" width="0.15" layer="21"/>
<pad name="1" x="0" y="0" drill="1" diameter="1.75" shape="square"/>
<pad name="2" x="0" y="-2.54" drill="1" diameter="1.75" shape="round"/>
<pad name="3" x="0" y="-5.08" drill="1" diameter="1.75" shape="round"/>
<pad name="4" x="0" y="-7.62" drill="1" diameter="1.75" shape="round"/>
<pad name="5" x="0" y="-10.16" drill="1" diameter="1.75" shape="round"/>
<pad name="6" x="0" y="-12.7" drill="1" diameter="1.75" shape="round"/>
<pad name="7" x="0" y="-15.24" drill="1" diameter="1.75" shape="round"/>
<pad name="8" x="0" y="-17.78" drill="1" diameter="1.75" shape="round"/>
<pad name="9" x="0" y="-20.32" drill="1" diameter="1.75" shape="round"/>
<pad name="10" x="0" y="-22.86" drill="1" diameter="1.75" shape="round"/>
<pad name="11" x="0" y="-25.4" drill="1" diameter="1.75" shape="round"/>
<pad name="12" x="0" y="-27.94" drill="1" diameter="1.75" shape="round"/>
<pad name="13" x="0" y="-30.48" drill="1" diameter="1.75" shape="round"/>
<pad name="14" x="0" y="-33.02" drill="1" diameter="1.75" shape="round"/>
<pad name="15" x="0" y="-35.56" drill="1" diameter="1.75" shape="round"/>
<pad name="16" x="0" y="-38.1" drill="1" diameter="1.75" shape="round"/>
<pad name="17" x="0" y="-40.64" drill="1" diameter="1.75" shape="round"/>
<pad name="18" x="0" y="-43.18" drill="1" diameter="1.75" shape="round"/>
<pad name="19" x="0" y="-45.72" drill="1" diameter="1.75" shape="round"/>
<pad name="20" x="0" y="-48.26" drill="1" diameter="1.75" shape="round"/>
<pad name="21" x="0" y="-50.8" drill="1" diameter="1.75" shape="round"/>
<pad name="22" x="0" y="-53.34" drill="1" diameter="1.75" shape="round"/>
<text x="-1.35" y="2.07" size="1.0" layer="25">&gt;NAME</text>
<text x="-1.35" y="-56.41" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="HDR_1X8">
<wire x1="-1.35" y1="1.27" x2="1.35" y2="1.27" width="0.15" layer="21"/>
<wire x1="1.35" y1="1.27" x2="1.35" y2="-19.05" width="0.15" layer="21"/>
<wire x1="1.35" y1="-19.05" x2="-1.35" y2="-19.05" width="0.15" layer="21"/>
<wire x1="-1.35" y1="-19.05" x2="-1.35" y2="1.27" width="0.15" layer="21"/>
<pad name="1" x="0" y="0" drill="1" diameter="1.75" shape="square"/>
<pad name="2" x="0" y="-2.54" drill="1" diameter="1.75" shape="round"/>
<pad name="3" x="0" y="-5.08" drill="1" diameter="1.75" shape="round"/>
<pad name="4" x="0" y="-7.62" drill="1" diameter="1.75" shape="round"/>
<pad name="5" x="0" y="-10.16" drill="1" diameter="1.75" shape="round"/>
<pad name="6" x="0" y="-12.7" drill="1" diameter="1.75" shape="round"/>
<pad name="7" x="0" y="-15.24" drill="1" diameter="1.75" shape="round"/>
<pad name="8" x="0" y="-17.78" drill="1" diameter="1.75" shape="round"/>
<text x="-1.35" y="2.07" size="1.0" layer="25">&gt;NAME</text>
<text x="-1.35" y="-20.85" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="HDR_1X5">
<wire x1="-1.35" y1="1.27" x2="1.35" y2="1.27" width="0.15" layer="21"/>
<wire x1="1.35" y1="1.27" x2="1.35" y2="-11.43" width="0.15" layer="21"/>
<wire x1="1.35" y1="-11.43" x2="-1.35" y2="-11.43" width="0.15" layer="21"/>
<wire x1="-1.35" y1="-11.43" x2="-1.35" y2="1.27" width="0.15" layer="21"/>
<pad name="1" x="0" y="0" drill="1" diameter="1.75" shape="square"/>
<pad name="2" x="0" y="-2.54" drill="1" diameter="1.75" shape="round"/>
<pad name="3" x="0" y="-5.08" drill="1" diameter="1.75" shape="round"/>
<pad name="4" x="0" y="-7.62" drill="1" diameter="1.75" shape="round"/>
<pad name="5" x="0" y="-10.16" drill="1" diameter="1.75" shape="round"/>
<text x="-1.35" y="2.07" size="1.0" layer="25">&gt;NAME</text>
<text x="-1.35" y="-13.23" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="HDR_1X4">
<wire x1="-1.35" y1="1.27" x2="1.35" y2="1.27" width="0.15" layer="21"/>
<wire x1="1.35" y1="1.27" x2="1.35" y2="-8.89" width="0.15" layer="21"/>
<wire x1="1.35" y1="-8.89" x2="-1.35" y2="-8.89" width="0.15" layer="21"/>
<wire x1="-1.35" y1="-8.89" x2="-1.35" y2="1.27" width="0.15" layer="21"/>
<pad name="1" x="0" y="0" drill="1" diameter="1.75" shape="square"/>
<pad name="2" x="0" y="-2.54" drill="1" diameter="1.75" shape="round"/>
<pad name="3" x="0" y="-5.08" drill="1" diameter="1.75" shape="round"/>
<pad name="4" x="0" y="-7.62" drill="1" diameter="1.75" shape="round"/>
<text x="-1.35" y="2.07" size="1.0" layer="25">&gt;NAME</text>
<text x="-1.35" y="-10.69" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="HDR_1X3">
<wire x1="-1.35" y1="1.27" x2="1.35" y2="1.27" width="0.15" layer="21"/>
<wire x1="1.35" y1="1.27" x2="1.35" y2="-6.35" width="0.15" layer="21"/>
<wire x1="1.35" y1="-6.35" x2="-1.35" y2="-6.35" width="0.15" layer="21"/>
<wire x1="-1.35" y1="-6.35" x2="-1.35" y2="1.27" width="0.15" layer="21"/>
<pad name="1" x="0" y="0" drill="1" diameter="1.75" shape="square"/>
<pad name="2" x="0" y="-2.54" drill="1" diameter="1.75" shape="round"/>
<pad name="3" x="0" y="-5.08" drill="1" diameter="1.75" shape="round"/>
<text x="-1.35" y="2.07" size="1.0" layer="25">&gt;NAME</text>
<text x="-1.35" y="-8.15" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="KF301_2P_508">
<wire x1="-4" y1="3" x2="4" y2="3" width="0.15" layer="21"/>
<wire x1="4" y1="3" x2="4" y2="-8.08" width="0.15" layer="21"/>
<wire x1="4" y1="-8.08" x2="-4" y2="-8.08" width="0.15" layer="21"/>
<wire x1="-4" y1="-8.08" x2="-4" y2="3" width="0.15" layer="21"/>
<pad name="1" x="0" y="0" drill="1.3" diameter="2.4" shape="square"/>
<pad name="2" x="0" y="-5.08" drill="1.3" diameter="2.4" shape="round"/>
<text x="-4" y="3.6" size="1.0" layer="25">&gt;NAME</text>
<text x="-4" y="-9.8" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="CAP_ELEC_10MM_P500">
<circle x="0" y="0" radius="5" width="0.15" layer="21"/>
<text x="-3.75" y="0.9" size="1.27" layer="21">+</text>
<pad name="1" x="-2.5" y="0" drill="1" diameter="2" shape="square"/>
<pad name="2" x="2.5" y="0" drill="1" diameter="2" shape="round"/>
<text x="-5" y="5.8" size="1.0" layer="25">&gt;NAME</text>
<text x="-5" y="-6.8" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="CAP_RAD_5MM_P254">
<circle x="0" y="0" radius="2.6" width="0.15" layer="21"/>
<text x="-1.95" y="0.9" size="1.27" layer="21">+</text>
<pad name="1" x="-1.27" y="0" drill="0.85" diameter="1.65" shape="square"/>
<pad name="2" x="1.27" y="0" drill="0.85" diameter="1.65" shape="round"/>
<text x="-2.6" y="3.4" size="1.0" layer="25">&gt;NAME</text>
<text x="-2.6" y="-4.4" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="CAP_DISC_P254">
<circle x="0" y="0" radius="2" width="0.15" layer="21"/>
<pad name="1" x="-1.27" y="0" drill="0.85" diameter="1.65" shape="round"/>
<pad name="2" x="1.27" y="0" drill="0.85" diameter="1.65" shape="round"/>
<text x="-2" y="2.8" size="1.0" layer="25">&gt;NAME</text>
<text x="-2" y="-3.8" size="0.8" layer="27">&gt;VALUE</text>
</package>
<package name="RES_AXIAL_762">
<wire x1="-2.5" y1="1.1" x2="2.5" y2="1.1" width="0.15" layer="21"/>
<wire x1="2.5" y1="1.1" x2="2.5" y2="-1.1" width="0.15" layer="21"/>
<wire x1="2.5" y1="-1.1" x2="-2.5" y2="-1.1" width="0.15" layer="21"/>
<wire x1="-2.5" y1="-1.1" x2="-2.5" y2="1.1" width="0.15" layer="21"/>
<pad name="1" x="-3.81" y="0" drill="0.9" diameter="1.75" shape="square"/>
<pad name="2" x="3.81" y="0" drill="0.9" diameter="1.75" shape="round"/>
<text x="-2.5" y="1.6" size="1.0" layer="25">&gt;NAME</text>
<text x="-2.5" y="-2.6" size="0.8" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="SYM_ESP32S3_LEFT_22P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="55.88" width="0.254" layer="94"/>
<wire x1="15.24" y1="55.88" x2="0" y2="55.88" width="0.254" layer="94"/>
<wire x1="0" y1="55.88" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="3V3_1" x="-5.08" y="53.34" length="middle" direction="pas"/>
<pin name="3V3_2" x="-5.08" y="50.8" length="middle" direction="pas"/>
<pin name="RST" x="-5.08" y="48.26" length="middle" direction="pas"/>
<pin name="GPIO4_FLAME" x="-5.08" y="45.72" length="middle" direction="pas"/>
<pin name="GPIO5_RLY_FAN" x="-5.08" y="43.18" length="middle" direction="pas"/>
<pin name="GPIO6_RLY_ATOM" x="-5.08" y="40.64" length="middle" direction="pas"/>
<pin name="GPIO7_BUZZER" x="-5.08" y="38.1" length="middle" direction="pas"/>
<pin name="GPIO15_LORA_DIO0" x="-5.08" y="35.56" length="middle" direction="pas"/>
<pin name="GPIO16_NC" x="-5.08" y="33.02" length="middle" direction="pas"/>
<pin name="GPIO17_RADAR_TX" x="-5.08" y="30.48" length="middle" direction="pas"/>
<pin name="GPIO18_RADAR_RX" x="-5.08" y="27.94" length="middle" direction="pas"/>
<pin name="GPIO8_SDA" x="-5.08" y="25.4" length="middle" direction="pas"/>
<pin name="GPIO3_STRAP" x="-5.08" y="22.86" length="middle" direction="pas"/>
<pin name="GPIO46_STRAP" x="-5.08" y="20.32" length="middle" direction="pas"/>
<pin name="GPIO9_SCL" x="-5.08" y="17.78" length="middle" direction="pas"/>
<pin name="GPIO10_LORA_CS" x="-5.08" y="15.24" length="middle" direction="pas"/>
<pin name="GPIO11_LORA_MOSI" x="-5.08" y="12.7" length="middle" direction="pas"/>
<pin name="GPIO12_LORA_SCK" x="-5.08" y="10.16" length="middle" direction="pas"/>
<pin name="GPIO13_LORA_MISO" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="GPIO14_LORA_RST" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="5V0_IN" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="GND_L" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="56.68" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_ESP32S3_RIGHT_22P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="55.88" width="0.254" layer="94"/>
<wire x1="15.24" y1="55.88" x2="0" y2="55.88" width="0.254" layer="94"/>
<wire x1="0" y1="55.88" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="GND_R1" x="-5.08" y="53.34" length="middle" direction="pas"/>
<pin name="U0TXD" x="-5.08" y="50.8" length="middle" direction="pas"/>
<pin name="U0RXD" x="-5.08" y="48.26" length="middle" direction="pas"/>
<pin name="GPIO1" x="-5.08" y="45.72" length="middle" direction="pas"/>
<pin name="GPIO2" x="-5.08" y="43.18" length="middle" direction="pas"/>
<pin name="GPIO42" x="-5.08" y="40.64" length="middle" direction="pas"/>
<pin name="GPIO41" x="-5.08" y="38.1" length="middle" direction="pas"/>
<pin name="GPIO40" x="-5.08" y="35.56" length="middle" direction="pas"/>
<pin name="GPIO39" x="-5.08" y="33.02" length="middle" direction="pas"/>
<pin name="GPIO38" x="-5.08" y="30.48" length="middle" direction="pas"/>
<pin name="GPIO37_PSRAM" x="-5.08" y="27.94" length="middle" direction="pas"/>
<pin name="GPIO36_PSRAM" x="-5.08" y="25.4" length="middle" direction="pas"/>
<pin name="GPIO35_PSRAM" x="-5.08" y="22.86" length="middle" direction="pas"/>
<pin name="GPIO0_BOOT" x="-5.08" y="20.32" length="middle" direction="pas"/>
<pin name="GPIO45_STRAP" x="-5.08" y="17.78" length="middle" direction="pas"/>
<pin name="GPIO48" x="-5.08" y="15.24" length="middle" direction="pas"/>
<pin name="GPIO47" x="-5.08" y="12.7" length="middle" direction="pas"/>
<pin name="GPIO21_NEOPIX" x="-5.08" y="10.16" length="middle" direction="pas"/>
<pin name="GPIO20_USBD_P" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="GPIO19_USBD_N" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="GND_R2" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="GND_R3" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="56.68" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_KF301_PWR_2P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="5.08" width="0.254" layer="94"/>
<wire x1="15.24" y1="5.08" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="P1" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="P2" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="5.88" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_AMS1117_MOD_3P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="7.62" width="0.254" layer="94"/>
<wire x1="15.24" y1="7.62" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="VIN_5V" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="VOUT_3V3" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="8.42" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_LORA_UNI_8P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="20.32" width="0.254" layer="94"/>
<wire x1="15.24" y1="20.32" x2="0" y2="20.32" width="0.254" layer="94"/>
<wire x1="0" y1="20.32" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="VCC_3V3" x="-5.08" y="17.78" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="15.24" length="middle" direction="pas"/>
<pin name="SCK" x="-5.08" y="12.7" length="middle" direction="pas"/>
<pin name="MISO" x="-5.08" y="10.16" length="middle" direction="pas"/>
<pin name="MOSI" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="CS_NSS" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="RST" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="DIO0" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="21.12" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_LORA_RA02_L8">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="20.32" width="0.254" layer="94"/>
<wire x1="15.24" y1="20.32" x2="0" y2="20.32" width="0.254" layer="94"/>
<wire x1="0" y1="20.32" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="GND1" x="-5.08" y="17.78" length="middle" direction="pas"/>
<pin name="VCC_3V3" x="-5.08" y="15.24" length="middle" direction="pas"/>
<pin name="RST" x="-5.08" y="12.7" length="middle" direction="pas"/>
<pin name="DIO0" x="-5.08" y="10.16" length="middle" direction="pas"/>
<pin name="DIO1" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="DIO2" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="DIO3" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="GND2" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="21.12" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_LORA_RA02_R8">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="20.32" width="0.254" layer="94"/>
<wire x1="15.24" y1="20.32" x2="0" y2="20.32" width="0.254" layer="94"/>
<wire x1="0" y1="20.32" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="GND3" x="-5.08" y="17.78" length="middle" direction="pas"/>
<pin name="ANT_NC" x="-5.08" y="15.24" length="middle" direction="pas"/>
<pin name="NSS" x="-5.08" y="12.7" length="middle" direction="pas"/>
<pin name="MOSI" x="-5.08" y="10.16" length="middle" direction="pas"/>
<pin name="MISO" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="SCK" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="DIO5" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="DIO4" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="21.12" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_I2C_SENSOR_4P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="10.16" width="0.254" layer="94"/>
<wire x1="15.24" y1="10.16" x2="0" y2="10.16" width="0.254" layer="94"/>
<wire x1="0" y1="10.16" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="VCC_3V3" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="SCL" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="SDA" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="10.96" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_OLED_GND_FIRST_4P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="10.16" width="0.254" layer="94"/>
<wire x1="15.24" y1="10.16" x2="0" y2="10.16" width="0.254" layer="94"/>
<wire x1="0" y1="10.16" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="GND" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="VCC_3V3" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="SCL" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="SDA" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="10.96" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_RADAR_LD2410C_5P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="12.7" width="0.254" layer="94"/>
<wire x1="15.24" y1="12.7" x2="0" y2="12.7" width="0.254" layer="94"/>
<wire x1="0" y1="12.7" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="RADAR_TX" x="-5.08" y="10.16" length="middle" direction="pas"/>
<pin name="RADAR_RX" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="OUT_NC" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="VCC_5V" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="13.5" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_FLAME_SENSOR_4P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="10.16" width="0.254" layer="94"/>
<wire x1="15.24" y1="10.16" x2="0" y2="10.16" width="0.254" layer="94"/>
<wire x1="0" y1="10.16" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="VCC_3V3" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="DO" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="AO_NC" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="10.96" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_RELAY_2CH_4P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="10.16" width="0.254" layer="94"/>
<wire x1="15.24" y1="10.16" x2="0" y2="10.16" width="0.254" layer="94"/>
<wire x1="0" y1="10.16" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="GND" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="IN1_FAN" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="IN2_ATOM" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="VCC_5V" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="10.96" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_BUZZER_3P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="7.62" width="0.254" layer="94"/>
<wire x1="15.24" y1="7.62" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="VCC_3V3" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="IO_SIG" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="8.42" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_WS2812B_3P">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="7.62" width="0.254" layer="94"/>
<wire x1="15.24" y1="7.62" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="VCC_5V" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="DIN" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="GND" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="8.42" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_CAP_BULK_1000UF">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="5.08" width="0.254" layer="94"/>
<wire x1="15.24" y1="5.08" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="POS" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="NEG" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="5.88" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_CAP_POL_254">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="5.08" width="0.254" layer="94"/>
<wire x1="15.24" y1="5.08" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="POS" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="NEG" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="5.88" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_CAP_CER_254">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="5.08" width="0.254" layer="94"/>
<wire x1="15.24" y1="5.08" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="P1" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="P2" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="5.88" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SYM_RES_330R">
<wire x1="0" y1="-2.54" x2="15.24" y2="-2.54" width="0.254" layer="94"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="5.08" width="0.254" layer="94"/>
<wire x1="15.24" y1="5.08" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="-2.54" width="0.254" layer="94"/>
<pin name="P1" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="P2" x="-5.08" y="0" length="middle" direction="pas"/>
<text x="0" y="5.88" size="1.5" layer="95">&gt;NAME</text>
<text x="0" y="-4.74" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
</symbols><devicesets>
<deviceset name="ESP32S3_LEFT_22P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_ESP32S3_LEFT_22P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X22"><connects>
<connect gate="G$1" pin="3V3_1" pad="1"/>
<connect gate="G$1" pin="3V3_2" pad="2"/>
<connect gate="G$1" pin="RST" pad="3"/>
<connect gate="G$1" pin="GPIO4_FLAME" pad="4"/>
<connect gate="G$1" pin="GPIO5_RLY_FAN" pad="5"/>
<connect gate="G$1" pin="GPIO6_RLY_ATOM" pad="6"/>
<connect gate="G$1" pin="GPIO7_BUZZER" pad="7"/>
<connect gate="G$1" pin="GPIO15_LORA_DIO0" pad="8"/>
<connect gate="G$1" pin="GPIO16_NC" pad="9"/>
<connect gate="G$1" pin="GPIO17_RADAR_TX" pad="10"/>
<connect gate="G$1" pin="GPIO18_RADAR_RX" pad="11"/>
<connect gate="G$1" pin="GPIO8_SDA" pad="12"/>
<connect gate="G$1" pin="GPIO3_STRAP" pad="13"/>
<connect gate="G$1" pin="GPIO46_STRAP" pad="14"/>
<connect gate="G$1" pin="GPIO9_SCL" pad="15"/>
<connect gate="G$1" pin="GPIO10_LORA_CS" pad="16"/>
<connect gate="G$1" pin="GPIO11_LORA_MOSI" pad="17"/>
<connect gate="G$1" pin="GPIO12_LORA_SCK" pad="18"/>
<connect gate="G$1" pin="GPIO13_LORA_MISO" pad="19"/>
<connect gate="G$1" pin="GPIO14_LORA_RST" pad="20"/>
<connect gate="G$1" pin="5V0_IN" pad="21"/>
<connect gate="G$1" pin="GND_L" pad="22"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="ESP32S3_RIGHT_22P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_ESP32S3_RIGHT_22P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X22"><connects>
<connect gate="G$1" pin="GND_R1" pad="1"/>
<connect gate="G$1" pin="U0TXD" pad="2"/>
<connect gate="G$1" pin="U0RXD" pad="3"/>
<connect gate="G$1" pin="GPIO1" pad="4"/>
<connect gate="G$1" pin="GPIO2" pad="5"/>
<connect gate="G$1" pin="GPIO42" pad="6"/>
<connect gate="G$1" pin="GPIO41" pad="7"/>
<connect gate="G$1" pin="GPIO40" pad="8"/>
<connect gate="G$1" pin="GPIO39" pad="9"/>
<connect gate="G$1" pin="GPIO38" pad="10"/>
<connect gate="G$1" pin="GPIO37_PSRAM" pad="11"/>
<connect gate="G$1" pin="GPIO36_PSRAM" pad="12"/>
<connect gate="G$1" pin="GPIO35_PSRAM" pad="13"/>
<connect gate="G$1" pin="GPIO0_BOOT" pad="14"/>
<connect gate="G$1" pin="GPIO45_STRAP" pad="15"/>
<connect gate="G$1" pin="GPIO48" pad="16"/>
<connect gate="G$1" pin="GPIO47" pad="17"/>
<connect gate="G$1" pin="GPIO21_NEOPIX" pad="18"/>
<connect gate="G$1" pin="GPIO20_USBD_P" pad="19"/>
<connect gate="G$1" pin="GPIO19_USBD_N" pad="20"/>
<connect gate="G$1" pin="GND_R2" pad="21"/>
<connect gate="G$1" pin="GND_R3" pad="22"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="KF301_PWR_2P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_KF301_PWR_2P" x="0" y="0"/></gates>
<devices><device name="" package="KF301_2P_508"><connects>
<connect gate="G$1" pin="P1" pad="1"/>
<connect gate="G$1" pin="P2" pad="2"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="AMS1117_MOD_3P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_AMS1117_MOD_3P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X3"><connects>
<connect gate="G$1" pin="VIN_5V" pad="1"/>
<connect gate="G$1" pin="GND" pad="2"/>
<connect gate="G$1" pin="VOUT_3V3" pad="3"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="LORA_UNI_8P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_LORA_UNI_8P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X8"><connects>
<connect gate="G$1" pin="VCC_3V3" pad="1"/>
<connect gate="G$1" pin="GND" pad="2"/>
<connect gate="G$1" pin="SCK" pad="3"/>
<connect gate="G$1" pin="MISO" pad="4"/>
<connect gate="G$1" pin="MOSI" pad="5"/>
<connect gate="G$1" pin="CS_NSS" pad="6"/>
<connect gate="G$1" pin="RST" pad="7"/>
<connect gate="G$1" pin="DIO0" pad="8"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="LORA_RA02_L8" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_LORA_RA02_L8" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X8"><connects>
<connect gate="G$1" pin="GND1" pad="1"/>
<connect gate="G$1" pin="VCC_3V3" pad="2"/>
<connect gate="G$1" pin="RST" pad="3"/>
<connect gate="G$1" pin="DIO0" pad="4"/>
<connect gate="G$1" pin="DIO1" pad="5"/>
<connect gate="G$1" pin="DIO2" pad="6"/>
<connect gate="G$1" pin="DIO3" pad="7"/>
<connect gate="G$1" pin="GND2" pad="8"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="LORA_RA02_R8" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_LORA_RA02_R8" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X8"><connects>
<connect gate="G$1" pin="GND3" pad="1"/>
<connect gate="G$1" pin="ANT_NC" pad="2"/>
<connect gate="G$1" pin="NSS" pad="3"/>
<connect gate="G$1" pin="MOSI" pad="4"/>
<connect gate="G$1" pin="MISO" pad="5"/>
<connect gate="G$1" pin="SCK" pad="6"/>
<connect gate="G$1" pin="DIO5" pad="7"/>
<connect gate="G$1" pin="DIO4" pad="8"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="I2C_SENSOR_4P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_I2C_SENSOR_4P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X4"><connects>
<connect gate="G$1" pin="VCC_3V3" pad="1"/>
<connect gate="G$1" pin="GND" pad="2"/>
<connect gate="G$1" pin="SCL" pad="3"/>
<connect gate="G$1" pin="SDA" pad="4"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="OLED_GND_FIRST_4P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_OLED_GND_FIRST_4P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X4"><connects>
<connect gate="G$1" pin="GND" pad="1"/>
<connect gate="G$1" pin="VCC_3V3" pad="2"/>
<connect gate="G$1" pin="SCL" pad="3"/>
<connect gate="G$1" pin="SDA" pad="4"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="RADAR_LD2410C_5P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_RADAR_LD2410C_5P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X5"><connects>
<connect gate="G$1" pin="RADAR_TX" pad="1"/>
<connect gate="G$1" pin="RADAR_RX" pad="2"/>
<connect gate="G$1" pin="OUT_NC" pad="3"/>
<connect gate="G$1" pin="GND" pad="4"/>
<connect gate="G$1" pin="VCC_5V" pad="5"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="FLAME_SENSOR_4P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_FLAME_SENSOR_4P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X4"><connects>
<connect gate="G$1" pin="VCC_3V3" pad="1"/>
<connect gate="G$1" pin="GND" pad="2"/>
<connect gate="G$1" pin="DO" pad="3"/>
<connect gate="G$1" pin="AO_NC" pad="4"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="RELAY_2CH_4P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_RELAY_2CH_4P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X4"><connects>
<connect gate="G$1" pin="GND" pad="1"/>
<connect gate="G$1" pin="IN1_FAN" pad="2"/>
<connect gate="G$1" pin="IN2_ATOM" pad="3"/>
<connect gate="G$1" pin="VCC_5V" pad="4"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="BUZZER_3P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_BUZZER_3P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X3"><connects>
<connect gate="G$1" pin="VCC_3V3" pad="1"/>
<connect gate="G$1" pin="IO_SIG" pad="2"/>
<connect gate="G$1" pin="GND" pad="3"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="WS2812B_3P" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_WS2812B_3P" x="0" y="0"/></gates>
<devices><device name="" package="HDR_1X3"><connects>
<connect gate="G$1" pin="VCC_5V" pad="1"/>
<connect gate="G$1" pin="DIN" pad="2"/>
<connect gate="G$1" pin="GND" pad="3"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="CAP_BULK_1000UF" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_CAP_BULK_1000UF" x="0" y="0"/></gates>
<devices><device name="" package="CAP_ELEC_10MM_P500"><connects>
<connect gate="G$1" pin="POS" pad="1"/>
<connect gate="G$1" pin="NEG" pad="2"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="CAP_POL_254" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_CAP_POL_254" x="0" y="0"/></gates>
<devices><device name="" package="CAP_RAD_5MM_P254"><connects>
<connect gate="G$1" pin="POS" pad="1"/>
<connect gate="G$1" pin="NEG" pad="2"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="CAP_CER_254" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_CAP_CER_254" x="0" y="0"/></gates>
<devices><device name="" package="CAP_DISC_P254"><connects>
<connect gate="G$1" pin="P1" pad="1"/>
<connect gate="G$1" pin="P2" pad="2"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
<deviceset name="RES_330R" prefix="J" uservalue="yes">
<gates><gate name="G$1" symbol="SYM_RES_330R" x="0" y="0"/></gates>
<devices><device name="" package="RES_AXIAL_762"><connects>
<connect gate="G$1" pin="P1" pad="1"/>
<connect gate="G$1" pin="P2" pad="2"/>
</connects><technologies><technology name=""/></technologies></device></devices></deviceset>
</devicesets></library></libraries><attributes/><variantdefs/>
<classes><class number="0" name="default" width="0.3" drill="0"/><class number="1" name="power" width="0.8" drill="0"/></classes>
<parts>
<part name="J_12V_IN" library="IDP_Lab_Safety" deviceset="KF301_PWR_2P" device="" value="12V_DC_IN"/>
<part name="J_SW_MST" library="IDP_Lab_Safety" deviceset="KF301_PWR_2P" device="" value="MASTER_SW"/>
<part name="J_BUCK_IN" library="IDP_Lab_Safety" deviceset="KF301_PWR_2P" device="" value="LM2596_IN"/>
<part name="J_BUCK_OUT" library="IDP_Lab_Safety" deviceset="KF301_PWR_2P" device="" value="5V_STAR_HUB"/>
<part name="J_LOAD_5V" library="IDP_Lab_Safety" deviceset="KF301_PWR_2P" device="" value="FAN_ATOM_PWR"/>
<part name="J_RELAY" library="IDP_Lab_Safety" deviceset="RELAY_2CH_4P" device="" value="2CH_5V_RELAY"/>
<part name="C5" library="IDP_Lab_Safety" deviceset="CAP_BULK_1000UF" device="" value="1000uF_25V"/>
<part name="R1" library="IDP_Lab_Safety" deviceset="RES_330R" device="" value="330R"/>
<part name="J_LED" library="IDP_Lab_Safety" deviceset="WS2812B_3P" device="" value="WS2812B_ARGB"/>
<part name="U1_L" library="IDP_Lab_Safety" deviceset="ESP32S3_LEFT_22P" device="" value="ESP32S3_L"/>
<part name="U1_R" library="IDP_Lab_Safety" deviceset="ESP32S3_RIGHT_22P" device="" value="ESP32S3_R_25M4"/>
<part name="U1_R_ALT" library="IDP_Lab_Safety" deviceset="ESP32S3_RIGHT_22P" device="" value="ESP32S3_R_22M8"/>
<part name="U_AMS" library="IDP_Lab_Safety" deviceset="AMS1117_MOD_3P" device="" value="AMS1117_3V3"/>
<part name="C1" library="IDP_Lab_Safety" deviceset="CAP_POL_254" device="" value="10uF"/>
<part name="C2" library="IDP_Lab_Safety" deviceset="CAP_CER_254" device="" value="100nF"/>
<part name="C3" library="IDP_Lab_Safety" deviceset="CAP_POL_254" device="" value="22uF_Tant"/>
<part name="C4" library="IDP_Lab_Safety" deviceset="CAP_CER_254" device="" value="100nF_LoRa"/>
<part name="U_LORA_L" library="IDP_Lab_Safety" deviceset="LORA_RA02_L8" device="" value="RA02_ADP_L"/>
<part name="U_LORA_R" library="IDP_Lab_Safety" deviceset="LORA_RA02_R8" device="" value="RA02_ADP_R"/>
<part name="J_LORA_UNI" library="IDP_Lab_Safety" deviceset="LORA_UNI_8P" device="" value="LORA_SPI_8P"/>
<part name="J_BME280" library="IDP_Lab_Safety" deviceset="I2C_SENSOR_4P" device="" value="BME280_I2C"/>
<part name="J_SGP30" library="IDP_Lab_Safety" deviceset="I2C_SENSOR_4P" device="" value="SGP30_I2C"/>
<part name="J_OLED_VCC" library="IDP_Lab_Safety" deviceset="I2C_SENSOR_4P" device="" value="OLED_VCC_1ST"/>
<part name="J_OLED_GND" library="IDP_Lab_Safety" deviceset="OLED_GND_FIRST_4P" device="" value="OLED_GND_1ST"/>
<part name="J_RADAR" library="IDP_Lab_Safety" deviceset="RADAR_LD2410C_5P" device="" value="HLK_LD2410C"/>
<part name="J_FLAME" library="IDP_Lab_Safety" deviceset="FLAME_SENSOR_4P" device="" value="IR_FLAME"/>
<part name="J_BUZZER" library="IDP_Lab_Safety" deviceset="BUZZER_3P" device="" value="ACT_BUZZER_3V3"/>
</parts><sheets><sheet><plain>
<text x="10" y="205" size="3.5" layer="94">IDP Smart Lab Safety Monitoring System - Main Node (ESP32-S3 Carrier PCB)</text>
</plain><instances>
<instance part="J_12V_IN" gate="G$1" x="15.24" y="177.8"/>
<instance part="J_SW_MST" gate="G$1" x="15.24" y="157.48"/>
<instance part="J_BUCK_IN" gate="G$1" x="15.24" y="137.16"/>
<instance part="J_BUCK_OUT" gate="G$1" x="15.24" y="116.84"/>
<instance part="J_LOAD_5V" gate="G$1" x="15.24" y="96.52"/>
<instance part="J_RELAY" gate="G$1" x="15.24" y="68.58"/>
<instance part="C5" gate="G$1" x="15.24" y="50.8"/>
<instance part="R1" gate="G$1" x="15.24" y="33.02"/>
<instance part="J_LED" gate="G$1" x="15.24" y="12.7"/>
<instance part="U1_L" gate="G$1" x="76.2" y="121.92"/>
<instance part="U1_R" gate="G$1" x="121.92" y="121.92"/>
<instance part="U1_R_ALT" gate="G$1" x="167.64" y="121.92"/>
<instance part="U_AMS" gate="G$1" x="215.9" y="175.26"/>
<instance part="C1" gate="G$1" x="215.9" y="160.02"/>
<instance part="C2" gate="G$1" x="215.9" y="144.78"/>
<instance part="C3" gate="G$1" x="215.9" y="129.54"/>
<instance part="C4" gate="G$1" x="215.9" y="114.3"/>
<instance part="U_LORA_L" gate="G$1" x="215.9" y="78.74"/>
<instance part="U_LORA_R" gate="G$1" x="261.62" y="78.74"/>
<instance part="J_LORA_UNI" gate="G$1" x="261.62" y="144.78"/>
<instance part="J_BME280" gate="G$1" x="76.2" y="78.74"/>
<instance part="J_SGP30" gate="G$1" x="76.2" y="58.42"/>
<instance part="J_OLED_VCC" gate="G$1" x="76.2" y="38.1"/>
<instance part="J_OLED_GND" gate="G$1" x="76.2" y="17.78"/>
<instance part="J_RADAR" gate="G$1" x="137.16" y="76.2"/>
<instance part="J_FLAME" gate="G$1" x="137.16" y="53.34"/>
<instance part="J_BUZZER" gate="G$1" x="137.16" y="33.02"/>
</instances><busses/><nets>
<net name="+12V_RAW" class="1">
<segment><pinref part="J_12V_IN" gate="G$1" pin="P1"/>
<wire x1="10.16" y1="180.34" x2="2.54" y2="180.34" width="0.1524" layer="91"/>
<label x="2.54" y="180.34" size="1.27" layer="95"/></segment>
<segment><pinref part="J_SW_MST" gate="G$1" pin="P1"/>
<wire x1="10.16" y1="160.02" x2="2.54" y2="160.02" width="0.1524" layer="91"/>
<label x="2.54" y="160.02" size="1.27" layer="95"/></segment>
</net>
<net name="+12V_SW" class="1">
<segment><pinref part="J_SW_MST" gate="G$1" pin="P2"/>
<wire x1="10.16" y1="157.48" x2="2.54" y2="157.48" width="0.1524" layer="91"/>
<label x="2.54" y="157.48" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BUCK_IN" gate="G$1" pin="P1"/>
<wire x1="10.16" y1="139.7" x2="2.54" y2="139.7" width="0.1524" layer="91"/>
<label x="2.54" y="139.7" size="1.27" layer="95"/></segment>
</net>
<net name="+5V_MAIN" class="1">
<segment><pinref part="J_BUCK_OUT" gate="G$1" pin="P1"/>
<wire x1="10.16" y1="119.38" x2="2.54" y2="119.38" width="0.1524" layer="91"/>
<label x="2.54" y="119.38" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_L" gate="G$1" pin="5V0_IN"/>
<wire x1="71.12" y1="124.46" x2="63.5" y2="124.46" width="0.1524" layer="91"/>
<label x="63.5" y="124.46" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LOAD_5V" gate="G$1" pin="P1"/>
<wire x1="10.16" y1="99.06" x2="2.54" y2="99.06" width="0.1524" layer="91"/>
<label x="2.54" y="99.06" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RELAY" gate="G$1" pin="VCC_5V"/>
<wire x1="10.16" y1="68.58" x2="2.54" y2="68.58" width="0.1524" layer="91"/>
<label x="2.54" y="68.58" size="1.27" layer="95"/></segment>
<segment><pinref part="C5" gate="G$1" pin="POS"/>
<wire x1="10.16" y1="53.34" x2="2.54" y2="53.34" width="0.1524" layer="91"/>
<label x="2.54" y="53.34" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LED" gate="G$1" pin="VCC_5V"/>
<wire x1="10.16" y1="17.78" x2="2.54" y2="17.78" width="0.1524" layer="91"/>
<label x="2.54" y="17.78" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RADAR" gate="G$1" pin="VCC_5V"/>
<wire x1="132.08" y1="76.2" x2="124.46" y2="76.2" width="0.1524" layer="91"/>
<label x="124.46" y="76.2" size="1.27" layer="95"/></segment>
<segment><pinref part="U_AMS" gate="G$1" pin="VIN_5V"/>
<wire x1="210.82" y1="180.34" x2="203.2" y2="180.34" width="0.1524" layer="91"/>
<label x="203.2" y="180.34" size="1.27" layer="95"/></segment>
<segment><pinref part="C1" gate="G$1" pin="POS"/>
<wire x1="210.82" y1="162.56" x2="203.2" y2="162.56" width="0.1524" layer="91"/>
<label x="203.2" y="162.56" size="1.27" layer="95"/></segment>
<segment><pinref part="C2" gate="G$1" pin="P1"/>
<wire x1="210.82" y1="147.32" x2="203.2" y2="147.32" width="0.1524" layer="91"/>
<label x="203.2" y="147.32" size="1.27" layer="95"/></segment>
</net>
<net name="+3V3_LORA" class="1">
<segment><pinref part="U_AMS" gate="G$1" pin="VOUT_3V3"/>
<wire x1="210.82" y1="175.26" x2="203.2" y2="175.26" width="0.1524" layer="91"/>
<label x="203.2" y="175.26" size="1.27" layer="95"/></segment>
<segment><pinref part="C3" gate="G$1" pin="POS"/>
<wire x1="210.82" y1="132.08" x2="203.2" y2="132.08" width="0.1524" layer="91"/>
<label x="203.2" y="132.08" size="1.27" layer="95"/></segment>
<segment><pinref part="C4" gate="G$1" pin="P1"/>
<wire x1="210.82" y1="116.84" x2="203.2" y2="116.84" width="0.1524" layer="91"/>
<label x="203.2" y="116.84" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_L" gate="G$1" pin="VCC_3V3"/>
<wire x1="210.82" y1="93.98" x2="203.2" y2="93.98" width="0.1524" layer="91"/>
<label x="203.2" y="93.98" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="VCC_3V3"/>
<wire x1="256.54" y1="162.56" x2="248.92" y2="162.56" width="0.1524" layer="91"/>
<label x="248.92" y="162.56" size="1.27" layer="95"/></segment>
</net>
<net name="+3V3_ESP" class="1">
<segment><pinref part="U1_L" gate="G$1" pin="3V3_1"/>
<wire x1="71.12" y1="175.26" x2="63.5" y2="175.26" width="0.1524" layer="91"/>
<label x="63.5" y="175.26" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_L" gate="G$1" pin="3V3_2"/>
<wire x1="71.12" y1="172.72" x2="63.5" y2="172.72" width="0.1524" layer="91"/>
<label x="63.5" y="172.72" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BME280" gate="G$1" pin="VCC_3V3"/>
<wire x1="71.12" y1="86.36" x2="63.5" y2="86.36" width="0.1524" layer="91"/>
<label x="63.5" y="86.36" size="1.27" layer="95"/></segment>
<segment><pinref part="J_SGP30" gate="G$1" pin="VCC_3V3"/>
<wire x1="71.12" y1="66.04" x2="63.5" y2="66.04" width="0.1524" layer="91"/>
<label x="63.5" y="66.04" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_VCC" gate="G$1" pin="VCC_3V3"/>
<wire x1="71.12" y1="45.72" x2="63.5" y2="45.72" width="0.1524" layer="91"/>
<label x="63.5" y="45.72" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_GND" gate="G$1" pin="VCC_3V3"/>
<wire x1="71.12" y1="22.86" x2="63.5" y2="22.86" width="0.1524" layer="91"/>
<label x="63.5" y="22.86" size="1.27" layer="95"/></segment>
<segment><pinref part="J_FLAME" gate="G$1" pin="VCC_3V3"/>
<wire x1="132.08" y1="60.96" x2="124.46" y2="60.96" width="0.1524" layer="91"/>
<label x="124.46" y="60.96" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BUZZER" gate="G$1" pin="VCC_3V3"/>
<wire x1="132.08" y1="38.1" x2="124.46" y2="38.1" width="0.1524" layer="91"/>
<label x="124.46" y="38.1" size="1.27" layer="95"/></segment>
</net>
<net name="GND" class="1">
<segment><pinref part="J_12V_IN" gate="G$1" pin="P2"/>
<wire x1="10.16" y1="177.8" x2="2.54" y2="177.8" width="0.1524" layer="91"/>
<label x="2.54" y="177.8" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BUCK_IN" gate="G$1" pin="P2"/>
<wire x1="10.16" y1="137.16" x2="2.54" y2="137.16" width="0.1524" layer="91"/>
<label x="2.54" y="137.16" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BUCK_OUT" gate="G$1" pin="P2"/>
<wire x1="10.16" y1="116.84" x2="2.54" y2="116.84" width="0.1524" layer="91"/>
<label x="2.54" y="116.84" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LOAD_5V" gate="G$1" pin="P2"/>
<wire x1="10.16" y1="96.52" x2="2.54" y2="96.52" width="0.1524" layer="91"/>
<label x="2.54" y="96.52" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RELAY" gate="G$1" pin="GND"/>
<wire x1="10.16" y1="76.2" x2="2.54" y2="76.2" width="0.1524" layer="91"/>
<label x="2.54" y="76.2" size="1.27" layer="95"/></segment>
<segment><pinref part="C5" gate="G$1" pin="NEG"/>
<wire x1="10.16" y1="50.8" x2="2.54" y2="50.8" width="0.1524" layer="91"/>
<label x="2.54" y="50.8" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LED" gate="G$1" pin="GND"/>
<wire x1="10.16" y1="12.7" x2="2.54" y2="12.7" width="0.1524" layer="91"/>
<label x="2.54" y="12.7" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_L" gate="G$1" pin="GND_L"/>
<wire x1="71.12" y1="121.92" x2="63.5" y2="121.92" width="0.1524" layer="91"/>
<label x="63.5" y="121.92" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R" gate="G$1" pin="GND_R1"/>
<wire x1="116.84" y1="175.26" x2="109.22" y2="175.26" width="0.1524" layer="91"/>
<label x="109.22" y="175.26" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R" gate="G$1" pin="GND_R2"/>
<wire x1="116.84" y1="124.46" x2="109.22" y2="124.46" width="0.1524" layer="91"/>
<label x="109.22" y="124.46" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R" gate="G$1" pin="GND_R3"/>
<wire x1="116.84" y1="121.92" x2="109.22" y2="121.92" width="0.1524" layer="91"/>
<label x="109.22" y="121.92" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GND_R1"/>
<wire x1="162.56" y1="175.26" x2="154.94" y2="175.26" width="0.1524" layer="91"/>
<label x="154.94" y="175.26" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GND_R2"/>
<wire x1="162.56" y1="124.46" x2="154.94" y2="124.46" width="0.1524" layer="91"/>
<label x="154.94" y="124.46" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GND_R3"/>
<wire x1="162.56" y1="121.92" x2="154.94" y2="121.92" width="0.1524" layer="91"/>
<label x="154.94" y="121.92" size="1.27" layer="95"/></segment>
<segment><pinref part="U_AMS" gate="G$1" pin="GND"/>
<wire x1="210.82" y1="177.8" x2="203.2" y2="177.8" width="0.1524" layer="91"/>
<label x="203.2" y="177.8" size="1.27" layer="95"/></segment>
<segment><pinref part="C1" gate="G$1" pin="NEG"/>
<wire x1="210.82" y1="160.02" x2="203.2" y2="160.02" width="0.1524" layer="91"/>
<label x="203.2" y="160.02" size="1.27" layer="95"/></segment>
<segment><pinref part="C2" gate="G$1" pin="P2"/>
<wire x1="210.82" y1="144.78" x2="203.2" y2="144.78" width="0.1524" layer="91"/>
<label x="203.2" y="144.78" size="1.27" layer="95"/></segment>
<segment><pinref part="C3" gate="G$1" pin="NEG"/>
<wire x1="210.82" y1="129.54" x2="203.2" y2="129.54" width="0.1524" layer="91"/>
<label x="203.2" y="129.54" size="1.27" layer="95"/></segment>
<segment><pinref part="C4" gate="G$1" pin="P2"/>
<wire x1="210.82" y1="114.3" x2="203.2" y2="114.3" width="0.1524" layer="91"/>
<label x="203.2" y="114.3" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_L" gate="G$1" pin="GND1"/>
<wire x1="210.82" y1="96.52" x2="203.2" y2="96.52" width="0.1524" layer="91"/>
<label x="203.2" y="96.52" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_L" gate="G$1" pin="GND2"/>
<wire x1="210.82" y1="78.74" x2="203.2" y2="78.74" width="0.1524" layer="91"/>
<label x="203.2" y="78.74" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_R" gate="G$1" pin="GND3"/>
<wire x1="256.54" y1="96.52" x2="248.92" y2="96.52" width="0.1524" layer="91"/>
<label x="248.92" y="96.52" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="GND"/>
<wire x1="256.54" y1="160.02" x2="248.92" y2="160.02" width="0.1524" layer="91"/>
<label x="248.92" y="160.02" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BME280" gate="G$1" pin="GND"/>
<wire x1="71.12" y1="83.82" x2="63.5" y2="83.82" width="0.1524" layer="91"/>
<label x="63.5" y="83.82" size="1.27" layer="95"/></segment>
<segment><pinref part="J_SGP30" gate="G$1" pin="GND"/>
<wire x1="71.12" y1="63.5" x2="63.5" y2="63.5" width="0.1524" layer="91"/>
<label x="63.5" y="63.5" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_VCC" gate="G$1" pin="GND"/>
<wire x1="71.12" y1="43.18" x2="63.5" y2="43.18" width="0.1524" layer="91"/>
<label x="63.5" y="43.18" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_GND" gate="G$1" pin="GND"/>
<wire x1="71.12" y1="25.4" x2="63.5" y2="25.4" width="0.1524" layer="91"/>
<label x="63.5" y="25.4" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RADAR" gate="G$1" pin="GND"/>
<wire x1="132.08" y1="78.74" x2="124.46" y2="78.74" width="0.1524" layer="91"/>
<label x="124.46" y="78.74" size="1.27" layer="95"/></segment>
<segment><pinref part="J_FLAME" gate="G$1" pin="GND"/>
<wire x1="132.08" y1="58.42" x2="124.46" y2="58.42" width="0.1524" layer="91"/>
<label x="124.46" y="58.42" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BUZZER" gate="G$1" pin="GND"/>
<wire x1="132.08" y1="33.02" x2="124.46" y2="33.02" width="0.1524" layer="91"/>
<label x="124.46" y="33.02" size="1.27" layer="95"/></segment>
</net>
<net name="I2C_SDA_IO8" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO8_SDA"/>
<wire x1="71.12" y1="147.32" x2="63.5" y2="147.32" width="0.1524" layer="91"/>
<label x="63.5" y="147.32" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BME280" gate="G$1" pin="SDA"/>
<wire x1="71.12" y1="78.74" x2="63.5" y2="78.74" width="0.1524" layer="91"/>
<label x="63.5" y="78.74" size="1.27" layer="95"/></segment>
<segment><pinref part="J_SGP30" gate="G$1" pin="SDA"/>
<wire x1="71.12" y1="58.42" x2="63.5" y2="58.42" width="0.1524" layer="91"/>
<label x="63.5" y="58.42" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_VCC" gate="G$1" pin="SDA"/>
<wire x1="71.12" y1="38.1" x2="63.5" y2="38.1" width="0.1524" layer="91"/>
<label x="63.5" y="38.1" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_GND" gate="G$1" pin="SDA"/>
<wire x1="71.12" y1="17.78" x2="63.5" y2="17.78" width="0.1524" layer="91"/>
<label x="63.5" y="17.78" size="1.27" layer="95"/></segment>
</net>
<net name="I2C_SCL_IO9" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO9_SCL"/>
<wire x1="71.12" y1="139.7" x2="63.5" y2="139.7" width="0.1524" layer="91"/>
<label x="63.5" y="139.7" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BME280" gate="G$1" pin="SCL"/>
<wire x1="71.12" y1="81.28" x2="63.5" y2="81.28" width="0.1524" layer="91"/>
<label x="63.5" y="81.28" size="1.27" layer="95"/></segment>
<segment><pinref part="J_SGP30" gate="G$1" pin="SCL"/>
<wire x1="71.12" y1="60.96" x2="63.5" y2="60.96" width="0.1524" layer="91"/>
<label x="63.5" y="60.96" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_VCC" gate="G$1" pin="SCL"/>
<wire x1="71.12" y1="40.64" x2="63.5" y2="40.64" width="0.1524" layer="91"/>
<label x="63.5" y="40.64" size="1.27" layer="95"/></segment>
<segment><pinref part="J_OLED_GND" gate="G$1" pin="SCL"/>
<wire x1="71.12" y1="20.32" x2="63.5" y2="20.32" width="0.1524" layer="91"/>
<label x="63.5" y="20.32" size="1.27" layer="95"/></segment>
</net>
<net name="UART_ESP_TX17_TO_RADAR_RX" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO17_RADAR_TX"/>
<wire x1="71.12" y1="152.4" x2="63.5" y2="152.4" width="0.1524" layer="91"/>
<label x="63.5" y="152.4" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RADAR" gate="G$1" pin="RADAR_RX"/>
<wire x1="132.08" y1="83.82" x2="124.46" y2="83.82" width="0.1524" layer="91"/>
<label x="124.46" y="83.82" size="1.27" layer="95"/></segment>
</net>
<net name="UART_RADAR_TX_TO_ESP_RX18" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO18_RADAR_RX"/>
<wire x1="71.12" y1="149.86" x2="63.5" y2="149.86" width="0.1524" layer="91"/>
<label x="63.5" y="149.86" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RADAR" gate="G$1" pin="RADAR_TX"/>
<wire x1="132.08" y1="86.36" x2="124.46" y2="86.36" width="0.1524" layer="91"/>
<label x="124.46" y="86.36" size="1.27" layer="95"/></segment>
</net>
<net name="LORA_CS_IO10" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO10_LORA_CS"/>
<wire x1="71.12" y1="137.16" x2="63.5" y2="137.16" width="0.1524" layer="91"/>
<label x="63.5" y="137.16" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_R" gate="G$1" pin="NSS"/>
<wire x1="256.54" y1="91.44" x2="248.92" y2="91.44" width="0.1524" layer="91"/>
<label x="248.92" y="91.44" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="CS_NSS"/>
<wire x1="256.54" y1="149.86" x2="248.92" y2="149.86" width="0.1524" layer="91"/>
<label x="248.92" y="149.86" size="1.27" layer="95"/></segment>
</net>
<net name="LORA_MOSI_IO11" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO11_LORA_MOSI"/>
<wire x1="71.12" y1="134.62" x2="63.5" y2="134.62" width="0.1524" layer="91"/>
<label x="63.5" y="134.62" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_R" gate="G$1" pin="MOSI"/>
<wire x1="256.54" y1="88.9" x2="248.92" y2="88.9" width="0.1524" layer="91"/>
<label x="248.92" y="88.9" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="MOSI"/>
<wire x1="256.54" y1="152.4" x2="248.92" y2="152.4" width="0.1524" layer="91"/>
<label x="248.92" y="152.4" size="1.27" layer="95"/></segment>
</net>
<net name="LORA_SCK_IO12" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO12_LORA_SCK"/>
<wire x1="71.12" y1="132.08" x2="63.5" y2="132.08" width="0.1524" layer="91"/>
<label x="63.5" y="132.08" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_R" gate="G$1" pin="SCK"/>
<wire x1="256.54" y1="83.82" x2="248.92" y2="83.82" width="0.1524" layer="91"/>
<label x="248.92" y="83.82" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="SCK"/>
<wire x1="256.54" y1="157.48" x2="248.92" y2="157.48" width="0.1524" layer="91"/>
<label x="248.92" y="157.48" size="1.27" layer="95"/></segment>
</net>
<net name="LORA_MISO_IO13" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO13_LORA_MISO"/>
<wire x1="71.12" y1="129.54" x2="63.5" y2="129.54" width="0.1524" layer="91"/>
<label x="63.5" y="129.54" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_R" gate="G$1" pin="MISO"/>
<wire x1="256.54" y1="86.36" x2="248.92" y2="86.36" width="0.1524" layer="91"/>
<label x="248.92" y="86.36" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="MISO"/>
<wire x1="256.54" y1="154.94" x2="248.92" y2="154.94" width="0.1524" layer="91"/>
<label x="248.92" y="154.94" size="1.27" layer="95"/></segment>
</net>
<net name="LORA_RST_IO14" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO14_LORA_RST"/>
<wire x1="71.12" y1="127" x2="63.5" y2="127" width="0.1524" layer="91"/>
<label x="63.5" y="127" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_L" gate="G$1" pin="RST"/>
<wire x1="210.82" y1="91.44" x2="203.2" y2="91.44" width="0.1524" layer="91"/>
<label x="203.2" y="91.44" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="RST"/>
<wire x1="256.54" y1="147.32" x2="248.92" y2="147.32" width="0.1524" layer="91"/>
<label x="248.92" y="147.32" size="1.27" layer="95"/></segment>
</net>
<net name="LORA_DIO0_IO15" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO15_LORA_DIO0"/>
<wire x1="71.12" y1="157.48" x2="63.5" y2="157.48" width="0.1524" layer="91"/>
<label x="63.5" y="157.48" size="1.27" layer="95"/></segment>
<segment><pinref part="U_LORA_L" gate="G$1" pin="DIO0"/>
<wire x1="210.82" y1="88.9" x2="203.2" y2="88.9" width="0.1524" layer="91"/>
<label x="203.2" y="88.9" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LORA_UNI" gate="G$1" pin="DIO0"/>
<wire x1="256.54" y1="144.78" x2="248.92" y2="144.78" width="0.1524" layer="91"/>
<label x="248.92" y="144.78" size="1.27" layer="95"/></segment>
</net>
<net name="FLAME_DO_IO4" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO4_FLAME"/>
<wire x1="71.12" y1="167.64" x2="63.5" y2="167.64" width="0.1524" layer="91"/>
<label x="63.5" y="167.64" size="1.27" layer="95"/></segment>
<segment><pinref part="J_FLAME" gate="G$1" pin="DO"/>
<wire x1="132.08" y1="55.88" x2="124.46" y2="55.88" width="0.1524" layer="91"/>
<label x="124.46" y="55.88" size="1.27" layer="95"/></segment>
</net>
<net name="RELAY_FAN_IO5" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO5_RLY_FAN"/>
<wire x1="71.12" y1="165.1" x2="63.5" y2="165.1" width="0.1524" layer="91"/>
<label x="63.5" y="165.1" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RELAY" gate="G$1" pin="IN1_FAN"/>
<wire x1="10.16" y1="73.66" x2="2.54" y2="73.66" width="0.1524" layer="91"/>
<label x="2.54" y="73.66" size="1.27" layer="95"/></segment>
</net>
<net name="RELAY_ATOM_IO6" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO6_RLY_ATOM"/>
<wire x1="71.12" y1="162.56" x2="63.5" y2="162.56" width="0.1524" layer="91"/>
<label x="63.5" y="162.56" size="1.27" layer="95"/></segment>
<segment><pinref part="J_RELAY" gate="G$1" pin="IN2_ATOM"/>
<wire x1="10.16" y1="71.12" x2="2.54" y2="71.12" width="0.1524" layer="91"/>
<label x="2.54" y="71.12" size="1.27" layer="95"/></segment>
</net>
<net name="BUZZER_IO7" class="0">
<segment><pinref part="U1_L" gate="G$1" pin="GPIO7_BUZZER"/>
<wire x1="71.12" y1="160.02" x2="63.5" y2="160.02" width="0.1524" layer="91"/>
<label x="63.5" y="160.02" size="1.27" layer="95"/></segment>
<segment><pinref part="J_BUZZER" gate="G$1" pin="IO_SIG"/>
<wire x1="132.08" y1="35.56" x2="124.46" y2="35.56" width="0.1524" layer="91"/>
<label x="124.46" y="35.56" size="1.27" layer="95"/></segment>
</net>
<net name="NEOPIX_MCU_IO21" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO21_NEOPIX"/>
<wire x1="116.84" y1="132.08" x2="109.22" y2="132.08" width="0.1524" layer="91"/>
<label x="109.22" y="132.08" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO21_NEOPIX"/>
<wire x1="162.56" y1="132.08" x2="154.94" y2="132.08" width="0.1524" layer="91"/>
<label x="154.94" y="132.08" size="1.27" layer="95"/></segment>
<segment><pinref part="R1" gate="G$1" pin="P1"/>
<wire x1="10.16" y1="35.56" x2="2.54" y2="35.56" width="0.1524" layer="91"/>
<label x="2.54" y="35.56" size="1.27" layer="95"/></segment>
</net>
<net name="NEOPIX_DIN_FILT" class="0">
<segment><pinref part="R1" gate="G$1" pin="P2"/>
<wire x1="10.16" y1="33.02" x2="2.54" y2="33.02" width="0.1524" layer="91"/>
<label x="2.54" y="33.02" size="1.27" layer="95"/></segment>
<segment><pinref part="J_LED" gate="G$1" pin="DIN"/>
<wire x1="10.16" y1="15.24" x2="2.54" y2="15.24" width="0.1524" layer="91"/>
<label x="2.54" y="15.24" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P2_U0TXD" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="U0TXD"/>
<wire x1="116.84" y1="172.72" x2="109.22" y2="172.72" width="0.1524" layer="91"/>
<label x="109.22" y="172.72" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="U0TXD"/>
<wire x1="162.56" y1="172.72" x2="154.94" y2="172.72" width="0.1524" layer="91"/>
<label x="154.94" y="172.72" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P3_U0RXD" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="U0RXD"/>
<wire x1="116.84" y1="170.18" x2="109.22" y2="170.18" width="0.1524" layer="91"/>
<label x="109.22" y="170.18" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="U0RXD"/>
<wire x1="162.56" y1="170.18" x2="154.94" y2="170.18" width="0.1524" layer="91"/>
<label x="154.94" y="170.18" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P4_GPIO1" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO1"/>
<wire x1="116.84" y1="167.64" x2="109.22" y2="167.64" width="0.1524" layer="91"/>
<label x="109.22" y="167.64" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO1"/>
<wire x1="162.56" y1="167.64" x2="154.94" y2="167.64" width="0.1524" layer="91"/>
<label x="154.94" y="167.64" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P5_GPIO2" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO2"/>
<wire x1="116.84" y1="165.1" x2="109.22" y2="165.1" width="0.1524" layer="91"/>
<label x="109.22" y="165.1" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO2"/>
<wire x1="162.56" y1="165.1" x2="154.94" y2="165.1" width="0.1524" layer="91"/>
<label x="154.94" y="165.1" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P6_GPIO42" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO42"/>
<wire x1="116.84" y1="162.56" x2="109.22" y2="162.56" width="0.1524" layer="91"/>
<label x="109.22" y="162.56" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO42"/>
<wire x1="162.56" y1="162.56" x2="154.94" y2="162.56" width="0.1524" layer="91"/>
<label x="154.94" y="162.56" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P7_GPIO41" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO41"/>
<wire x1="116.84" y1="160.02" x2="109.22" y2="160.02" width="0.1524" layer="91"/>
<label x="109.22" y="160.02" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO41"/>
<wire x1="162.56" y1="160.02" x2="154.94" y2="160.02" width="0.1524" layer="91"/>
<label x="154.94" y="160.02" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P8_GPIO40" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO40"/>
<wire x1="116.84" y1="157.48" x2="109.22" y2="157.48" width="0.1524" layer="91"/>
<label x="109.22" y="157.48" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO40"/>
<wire x1="162.56" y1="157.48" x2="154.94" y2="157.48" width="0.1524" layer="91"/>
<label x="154.94" y="157.48" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P9_GPIO39" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO39"/>
<wire x1="116.84" y1="154.94" x2="109.22" y2="154.94" width="0.1524" layer="91"/>
<label x="109.22" y="154.94" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO39"/>
<wire x1="162.56" y1="154.94" x2="154.94" y2="154.94" width="0.1524" layer="91"/>
<label x="154.94" y="154.94" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P10_GPIO38" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO38"/>
<wire x1="116.84" y1="152.4" x2="109.22" y2="152.4" width="0.1524" layer="91"/>
<label x="109.22" y="152.4" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO38"/>
<wire x1="162.56" y1="152.4" x2="154.94" y2="152.4" width="0.1524" layer="91"/>
<label x="154.94" y="152.4" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P11_GPIO37_PSRAM" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO37_PSRAM"/>
<wire x1="116.84" y1="149.86" x2="109.22" y2="149.86" width="0.1524" layer="91"/>
<label x="109.22" y="149.86" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO37_PSRAM"/>
<wire x1="162.56" y1="149.86" x2="154.94" y2="149.86" width="0.1524" layer="91"/>
<label x="154.94" y="149.86" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P12_GPIO36_PSRAM" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO36_PSRAM"/>
<wire x1="116.84" y1="147.32" x2="109.22" y2="147.32" width="0.1524" layer="91"/>
<label x="109.22" y="147.32" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO36_PSRAM"/>
<wire x1="162.56" y1="147.32" x2="154.94" y2="147.32" width="0.1524" layer="91"/>
<label x="154.94" y="147.32" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P13_GPIO35_PSRAM" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO35_PSRAM"/>
<wire x1="116.84" y1="144.78" x2="109.22" y2="144.78" width="0.1524" layer="91"/>
<label x="109.22" y="144.78" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO35_PSRAM"/>
<wire x1="162.56" y1="144.78" x2="154.94" y2="144.78" width="0.1524" layer="91"/>
<label x="154.94" y="144.78" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P14_GPIO0_BOOT" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO0_BOOT"/>
<wire x1="116.84" y1="142.24" x2="109.22" y2="142.24" width="0.1524" layer="91"/>
<label x="109.22" y="142.24" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO0_BOOT"/>
<wire x1="162.56" y1="142.24" x2="154.94" y2="142.24" width="0.1524" layer="91"/>
<label x="154.94" y="142.24" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P15_GPIO45_STRAP" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO45_STRAP"/>
<wire x1="116.84" y1="139.7" x2="109.22" y2="139.7" width="0.1524" layer="91"/>
<label x="109.22" y="139.7" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO45_STRAP"/>
<wire x1="162.56" y1="139.7" x2="154.94" y2="139.7" width="0.1524" layer="91"/>
<label x="154.94" y="139.7" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P16_GPIO48" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO48"/>
<wire x1="116.84" y1="137.16" x2="109.22" y2="137.16" width="0.1524" layer="91"/>
<label x="109.22" y="137.16" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO48"/>
<wire x1="162.56" y1="137.16" x2="154.94" y2="137.16" width="0.1524" layer="91"/>
<label x="154.94" y="137.16" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P17_GPIO47" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO47"/>
<wire x1="116.84" y1="134.62" x2="109.22" y2="134.62" width="0.1524" layer="91"/>
<label x="109.22" y="134.62" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO47"/>
<wire x1="162.56" y1="134.62" x2="154.94" y2="134.62" width="0.1524" layer="91"/>
<label x="154.94" y="134.62" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P19_GPIO20_USBD_P" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO20_USBD_P"/>
<wire x1="116.84" y1="129.54" x2="109.22" y2="129.54" width="0.1524" layer="91"/>
<label x="109.22" y="129.54" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO20_USBD_P"/>
<wire x1="162.56" y1="129.54" x2="154.94" y2="129.54" width="0.1524" layer="91"/>
<label x="154.94" y="129.54" size="1.27" layer="95"/></segment>
</net>
<net name="ESP_R_P20_GPIO19_USBD_N" class="0">
<segment><pinref part="U1_R" gate="G$1" pin="GPIO19_USBD_N"/>
<wire x1="116.84" y1="127" x2="109.22" y2="127" width="0.1524" layer="91"/>
<label x="109.22" y="127" size="1.27" layer="95"/></segment>
<segment><pinref part="U1_R_ALT" gate="G$1" pin="GPIO19_USBD_N"/>
<wire x1="162.56" y1="127" x2="154.94" y2="127" width="0.1524" layer="91"/>
<label x="154.94" y="127" size="1.27" layer="95"/></segment>
</net>
</nets></sheet></sheets></schematic></drawing></eagle>