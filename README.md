# How to Use This Library

This library supports 1,3,5,6,15,16 functions of modbus. 

**1.** Download file and include the source file in Core/Src/ModbusRTU_Slave.c and Core/Inc/ModbusRTU_Slave.h header file in your project.

**2.** Set UART peripheral settings in your project. 

:exclamation: Attention. This project uses USART2 peripheral. If you want to different UART peripheral then you should change the value of UART_HandleTypeDef in the ModbusRTU_Slave.c source file.

![image](https://user-images.githubusercontent.com/64977597/155136571-725a783a-b87b-41aa-a6a8-f7af2c698c48.png)


**3.** Set number of register, number of coil and slave id in the slave device.

![image](https://user-images.githubusercontent.com/64977597/155137609-518863a6-ff9a-4240-b51a-944c7636bd23.png)

**4.** Don't forget to write 

`HAL_UART_Receive_IT(&huart2 , &uartRxData , 1);`

in main source code as uart receiver interrupt is used in this library. 

**5.** You can write your register and coil values in the following arrays. Other than that, there is nothing you need to do. When the master sends a read request, the library will create the required package and respond to the master device. 

![image](https://user-images.githubusercontent.com/64977597/155137841-c1aec211-3c1f-4c65-b783-ef37dc45d929.png)


