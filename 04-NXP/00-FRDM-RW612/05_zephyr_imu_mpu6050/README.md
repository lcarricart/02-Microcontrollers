![Physical setup](physical_setup.jpeg)

# FRDM-RW612 Zephyr MPU6050 IMU Application

This project runs the Zephyr MPU6050 IMU (Inertial Measurement Unit) sample on the NXP FRDM-RW612 board.

Project location:

```text
C:\Users\luchi\source\repos\lcarricart\02-Microcontrollers\04-NXP\00-FRDM-RW612\05_zephyr_imu_mpu6050
```

Zephyr workspace used to build it:

```text
C:\Users\luchi\Github-Repository\not-repos\zephyrproject
```

---

## Hardware Connection

The MPU6050 is connected to the FRDM-RW612 Arduino I2C (Inter-Integrated Circuit) pins:

```text
FRDM-RW612 D18 / I2C SDA  -> MPU6050 SDA
FRDM-RW612 D19 / I2C SCL  -> MPU6050 SCL
FRDM-RW612 3.3V           -> MPU6050 VCC
FRDM-RW612 GND            -> MPU6050 GND
```

The schematic labels for these lines are:

```text
GPIO_16_FC2_I2C_SDA_ARD
GPIO_17_FC2_I2C_SCL_ARD
```

So the sensor is connected to the board's Arduino I2C path, which maps to the Flexcomm 2 I2C peripheral.

---

## Required Overlay File

The application uses this board overlay:

```text
boards\frdm_rw612_mpu6050.overlay
```

Expected content:

```dts
&arduino_i2c {
	status = "okay";
	clock-frequency = <100000>;

	mpu6050@68 {
		compatible = "invensense,mpu6050";
		reg = <0x68>;
		status = "okay";
	};
};
```

The default MPU6050 I2C address is usually:

```text
0x68
```

If the sensor module uses `AD0` pulled high, change the address to:

```dts
reg = <0x69>;
```

---

## Build the Application

Open a PowerShell terminal in VS Code and activate the Zephyr environment:

```powershell
cd C:\Users\luchi\Github-Repository\not-repos\zephyrproject
.\.venv\Scripts\Activate.ps1
```

Define the application path:

```powershell
$APP_DIR = "C:\Users\luchi\source\repos\lcarricart\02-Microcontrollers\04-NXP\00-FRDM-RW612\05_zephyr_imu_mpu6050"
```

Build with the overlay:

```powershell
west build -p always -b frdm_rw612 "$APP_DIR" -- "-DDTC_OVERLAY_FILE=$APP_DIR/boards/frdm_rw612_mpu6050.overlay"
```

---

## Flash the Board

After a successful build:

```powershell
west flash
```

---

## Open Serial Monitor

Open the board serial terminal using:

```text
115200 8N1
```

Meaning:

```text
115200 baud
8 data bits
no parity
1 stop bit
```

---

## Expected Output

The sample should periodically print MPU6050 sensor values, including:

```text
temperature
acceleration x/y/z
gyroscope x/y/z
```

The exact numbers depend on the physical orientation and movement of the IMU.

---

## Troubleshooting

If the build fails because `arduino_i2c` is unknown, replace the overlay with the hardware-specific Flexcomm 2 node:

```dts
&flexcomm2 {
	status = "okay";
	clock-frequency = <100000>;

	mpu6050@68 {
		compatible = "invensense,mpu6050";
		reg = <0x68>;
		status = "okay";
	};
};
```

If the app builds but does not read values:

```text
1. Confirm VCC is connected to 3.3V.
2. Confirm GND is connected to board GND.
3. Confirm SDA is connected to D18 / I2C SDA.
4. Confirm SCL is connected to D19 / I2C SCL.
5. Try I2C address 0x69 if 0x68 fails.
6. Keep jumper wires short.
7. Check whether the MPU6050 breakout board already includes I2C pull-up resistors.
```
