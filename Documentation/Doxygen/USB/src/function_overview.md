# Function Overview {#function_overview}

The following list provides a brief overview of all USB Component functions.

## USB Device {#usbd_functions}

\ref  usbd_coreFunctions

- \ref USBD_GetVersion &mdash; \copybrief USBD_GetVersion
- \ref USBD_Configured &mdash; \copybrief USBD_Configured
- \ref USBD_Connect &mdash; \copybrief USBD_Connect
- \ref USBD_Devicen_DisableRemoteWakeup  &mdash; \copybrief USBD_Devicen_DisableRemoteWakeup
- \ref USBD_Devicen_EnableRemoteWakeup  &mdash; \copybrief USBD_Devicen_EnableRemoteWakeup
- \ref USBD_Devicen_Endpoint0_InDataSent  &mdash; \copybrief USBD_Devicen_Endpoint0_InDataSent
- \ref USBD_Devicen_Endpoint0_OutDataReceived  &mdash; \copybrief USBD_Devicen_Endpoint0_OutDataReceived
- \ref USBD_Devicen_Endpoint0_SetupPacketProcessed  &mdash; \copybrief USBD_Devicen_Endpoint0_SetupPacketProcessed
- \ref USBD_Devicen_Endpoint0_SetupPacketReceived  &mdash; \copybrief USBD_Devicen_Endpoint0_SetupPacketReceived
- \ref USBD_Devicen_ConfigurationChanged  &mdash; \copybrief USBD_Devicen_ConfigurationChanged
- \ref USBD_Devicen_HighSpeedActivated &mdash; \copybrief USBD_Devicen_HighSpeedActivated
- \ref USBD_Devicen_Initialize &mdash; \copybrief USBD_Devicen_Initialize
- \ref USBD_Devicen_Reset &mdash; \copybrief USBD_Devicen_Reset
- \ref USBD_Devicen_Resumed  &mdash; \copybrief USBD_Devicen_Resumed
- \ref USBD_Devicen_Suspended  &mdash; \copybrief USBD_Devicen_Suspended
- \ref USBD_Devicen_Uninitialize  &mdash; \copybrief USBD_Devicen_Uninitialize
- \ref USBD_Devicen_VbusChanged &mdash; \copybrief USBD_Devicen_VbusChanged
- \ref USBD_Disconnect &mdash; \copybrief USBD_Disconnect
- \ref USBD_GetState &mdash; \copybrief USBD_GetState
- \ref USBD_SetSerialNumber &mdash; \copybrief USBD_SetSerialNumber
- \ref USBD_Initialize &mdash; \copybrief USBD_Initialize
- \ref USBD_Uninitialize &mdash; \copybrief USBD_Uninitialize

\ref  usbd_adcFunctions

- \ref USBD_ADC_ReadSamples  &mdash; \copybrief USBD_ADC_ReadSamples
- \ref USBD_ADC_ReceivedSamplesAvailable  &mdash; \copybrief USBD_ADC_ReceivedSamplesAvailable
- \ref USBD_ADC_SetMicrophoneVolumeRange  &mdash; \copybrief USBD_ADC_SetMicrophoneVolumeRange
- \ref USBD_ADC_SetSpeakerVolumeRange  &mdash; \copybrief USBD_ADC_SetSpeakerVolumeRange
- \ref USBD_ADC_WriteSamples  &mdash; \copybrief USBD_ADC_WriteSamples
- \ref USBD_ADC_WrittenSamplesPending  &mdash; \copybrief USBD_ADC_WrittenSamplesPending
- \ref USBD_ADCn_Initialize  &mdash; \copybrief USBD_ADCn_Initialize
- \ref USBD_ADCn_PlayStart  &mdash; \copybrief USBD_ADCn_PlayStart
- \ref USBD_ADCn_PlayStop  &mdash; \copybrief USBD_ADCn_PlayStop
- \ref USBD_ADCn_ReceivedSamples  &mdash; \copybrief USBD_ADCn_ReceivedSamples
- \ref USBD_ADCn_RecordStart  &mdash; \copybrief USBD_ADCn_RecordStart
- \ref USBD_ADCn_RecordStop  &mdash; \copybrief USBD_ADCn_RecordStop
- \ref USBD_ADCn_SetSpeakerMute  &mdash; \copybrief USBD_ADCn_SetSpeakerMute
- \ref USBD_ADCn_SetSpeakerVolume  &mdash; \copybrief USBD_ADCn_SetSpeakerVolume
- \ref USBD_ADCn_SetMicrophoneMute  &mdash; \copybrief USBD_ADCn_SetMicrophoneMute
- \ref USBD_ADCn_SetMicrophoneVolume  &mdash; \copybrief USBD_ADCn_SetMicrophoneVolume
- \ref USBD_ADCn_Uninitialize  &mdash; \copybrief USBD_ADCn_Uninitialize

\ref  usbd_cdcFunctions_acm

- \ref USBD_CDC_ACM_DataAvailable  &mdash; \copybrief USBD_CDC_ACM_DataAvailable
- \ref USBD_CDC_ACM_GetChar  &mdash; \copybrief USBD_CDC_ACM_GetChar
- \ref USBD_CDC_ACM_Notify_SerialState  &mdash; \copybrief USBD_CDC_ACM_Notify_SerialState
- \ref USBD_CDC_ACM_Notify_ResponseAvailable  &mdash; \copybrief USBD_CDC_ACM_Notify_ResponseAvailable
- \ref USBD_CDC_ACM_PutChar  &mdash; \copybrief USBD_CDC_ACM_PutChar
- \ref USBD_CDC_ACM_ReadData  &mdash; \copybrief USBD_CDC_ACM_ReadData
- \ref USBD_CDC_ACM_WriteData  &mdash; \copybrief USBD_CDC_ACM_WriteData
- \ref USBD_CDCn_ACM_DataReceived  &mdash; \copybrief USBD_CDCn_ACM_DataReceived
- \ref USBD_CDCn_ACM_DataSent  &mdash; \copybrief USBD_CDCn_ACM_DataSent
- \ref USBD_CDCn_ACM_GetEncapsulatedResponse  &mdash; \copybrief USBD_CDCn_ACM_GetEncapsulatedResponse
- \ref USBD_CDCn_ACM_GetLineCoding  &mdash; \copybrief USBD_CDCn_ACM_GetLineCoding
- \ref USBD_CDCn_ACM_Initialize  &mdash; \copybrief USBD_CDCn_ACM_Initialize
- \ref USBD_CDCn_ACM_Reset  &mdash; \copybrief USBD_CDCn_ACM_Reset
- \ref USBD_CDCn_ACM_SendEncapsulatedCommand  &mdash; \copybrief USBD_CDCn_ACM_SendEncapsulatedCommand
- \ref USBD_CDCn_ACM_SetControlLineState  &mdash; \copybrief USBD_CDCn_ACM_SetControlLineState
- \ref USBD_CDCn_ACM_SetLineCoding  &mdash; \copybrief USBD_CDCn_ACM_SetLineCoding
- \ref USBD_CDCn_ACM_Uninitialize  &mdash; \copybrief USBD_CDCn_ACM_Uninitialize

\ref  usbd_cdcFunctions_ncm

- \ref USBD_CDC_NCM_Notify_ConnectionSpeedChange  &mdash; \copybrief USBD_CDC_NCM_Notify_ConnectionSpeedChange
- \ref USBD_CDC_NCM_Notify_NetworkConnection  &mdash; \copybrief USBD_CDC_NCM_Notify_NetworkConnection
- \ref USBD_CDC_NCM_NTB_IN_CreateNDP  &mdash; \copybrief USBD_CDC_NCM_NTB_IN_CreateNDP
- \ref USBD_CDC_NCM_NTB_IN_Initialize  &mdash; \copybrief USBD_CDC_NCM_NTB_IN_Initialize
- \ref USBD_CDC_NCM_NTB_IN_IsSent  &mdash; \copybrief USBD_CDC_NCM_NTB_IN_IsSent
- \ref USBD_CDC_NCM_NTB_IN_RawSend  &mdash; \copybrief USBD_CDC_NCM_NTB_IN_RawSend
- \ref USBD_CDC_NCM_NTB_IN_Send  &mdash; \copybrief USBD_CDC_NCM_NTB_IN_Send
- \ref USBD_CDC_NCM_NTB_IN_WriteDatagram  &mdash; \copybrief USBD_CDC_NCM_NTB_IN_WriteDatagram
- \ref USBD_CDC_NCM_NTB_OUT_GetDatagramSize  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_GetDatagramSize
- \ref USBD_CDC_NCM_NTB_OUT_IsReceived  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_IsReceived
- \ref USBD_CDC_NCM_NTB_OUT_ProcessNDP  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_ProcessNDP
- \ref USBD_CDC_NCM_NTB_OUT_RawGetSize  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_RawGetSize
- \ref USBD_CDC_NCM_NTB_OUT_RawReceive  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_RawReceive
- \ref USBD_CDC_NCM_NTB_OUT_ReadDatagram  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_ReadDatagram
- \ref USBD_CDC_NCM_NTB_OUT_Release  &mdash; \copybrief USBD_CDC_NCM_NTB_OUT_Release
- \ref USBD_CDCn_NCM_GetCrcMode  &mdash; \copybrief USBD_CDCn_NCM_GetCrcMode
- \ref USBD_CDCn_NCM_GetEthernetPowerManagementPatternFilter  &mdash; \copybrief USBD_CDCn_NCM_GetEthernetPowerManagementPatternFilter
- \ref USBD_CDCn_NCM_GetEthernetStatistic  &mdash; \copybrief USBD_CDCn_NCM_GetEthernetStatistic
- \ref USBD_CDCn_NCM_GetMaxDatagramSize  &mdash; \copybrief USBD_CDCn_NCM_GetMaxDatagramSize
- \ref USBD_CDCn_NCM_GetNetAddress  &mdash; \copybrief USBD_CDCn_NCM_GetNetAddress
- \ref USBD_CDCn_NCM_GetNtbFormat  &mdash; \copybrief USBD_CDCn_NCM_GetNtbFormat
- \ref USBD_CDCn_NCM_GetNtbInputSize  &mdash; \copybrief USBD_CDCn_NCM_GetNtbInputSize
- \ref USBD_CDCn_NCM_GetNtbParameters  &mdash; \copybrief USBD_CDCn_NCM_GetNtbParameters
- \ref USBD_CDCn_NCM_Initialize  &mdash; \copybrief USBD_CDCn_NCM_Initialize
- \ref USBD_CDCn_NCM_NTB_IN_Sent  &mdash; \copybrief USBD_CDCn_NCM_NTB_IN_Sent
- \ref USBD_CDCn_NCM_NTB_OUT_Received  &mdash; \copybrief USBD_CDCn_NCM_NTB_OUT_Received
- \ref USBD_CDCn_NCM_Reset  &mdash; \copybrief USBD_CDCn_NCM_Reset
- \ref USBD_CDCn_NCM_SetCrcMode  &mdash; \copybrief USBD_CDCn_NCM_SetCrcMode
- \ref USBD_CDCn_NCM_SetEthernetMulticastFilters  &mdash; \copybrief USBD_CDCn_NCM_SetEthernetMulticastFilters
- \ref USBD_CDCn_NCM_SetEthernetPacketFilter  &mdash; \copybrief USBD_CDCn_NCM_SetEthernetPacketFilter
- \ref USBD_CDCn_NCM_SetEthernetPowerManagementPatternFilter  &mdash; \copybrief USBD_CDCn_NCM_SetEthernetPowerManagementPatternFilter
- \ref USBD_CDCn_NCM_SetMaxDatagramSize  &mdash; \copybrief USBD_CDCn_NCM_SetMaxDatagramSize
- \ref USBD_CDCn_NCM_SetNetAddress  &mdash; \copybrief USBD_CDCn_NCM_SetNetAddress
- \ref USBD_CDCn_NCM_SetNtbFormat  &mdash; \copybrief USBD_CDCn_NCM_SetNtbFormat
- \ref USBD_CDCn_NCM_SetNtbInputSize  &mdash; \copybrief USBD_CDCn_NCM_SetNtbInputSize
- \ref USBD_CDCn_NCM_Start  &mdash; \copybrief USBD_CDCn_NCM_Start
- \ref USBD_CDCn_NCM_Stop  &mdash; \copybrief USBD_CDCn_NCM_Stop
- \ref USBD_CDCn_NCM_Uninitialize  &mdash; \copybrief USBD_CDCn_NCM_Uninitialize

\ref  usbd_classFunctions

- \ref USBD_CustomClassn_Initialize  &mdash; \copybrief USBD_CustomClassn_Initialize
- \ref USBD_CustomClassn_Uninitialize  &mdash; \copybrief USBD_CustomClassn_Uninitialize
- \ref USBD_CustomClassn_Reset  &mdash; \copybrief USBD_CustomClassn_Reset
- \ref USBD_CustomClassn_EndpointStart  &mdash; \copybrief USBD_CustomClassn_EndpointStart
- \ref USBD_CustomClassn_EndpointStop  &mdash; \copybrief USBD_CustomClassn_EndpointStop
- \ref USBD_CustomClassn_Endpoint0_SetupPacketReceived  &mdash; \copybrief USBD_CustomClassn_Endpoint0_SetupPacketReceived
- \ref USBD_CustomClassn_Endpoint0_SetupPacketProcessed  &mdash; \copybrief USBD_CustomClassn_Endpoint0_SetupPacketProcessed
- \ref USBD_CustomClassn_Endpoint0_OutDataReceived  &mdash; \copybrief USBD_CustomClassn_Endpoint0_OutDataReceived
- \ref USBD_CustomClassn_Endpoint0_InDataSent  &mdash; \copybrief USBD_CustomClassn_Endpoint0_InDataSent
- \ref USBD_CustomClassn_Endpoint1_Event  &mdash;  Is called by the USB Device Core when an event happens on endpoint 1.  
  :  
  \ref USBD_CustomClassn_Endpoint15_Event  &mdash;  Is called by the USB Device Core when an event happens on endpoint 15.
- \ref USBD_EndpointRead  &mdash; \copybrief USBD_EndpointRead
- \ref USBD_EndpointReadGetResult  &mdash; \copybrief USBD_EndpointReadGetResult
- \ref USBD_EndpointWrite  &mdash; \copybrief USBD_EndpointWrite
- \ref USBD_EndpointWriteGetResult  &mdash; \copybrief USBD_EndpointWriteGetResult
- \ref USBD_EndpointStall  &mdash; \copybrief USBD_EndpointStall
- \ref USBD_EndpointAbort  &mdash; \copybrief USBD_EndpointAbort

\ref  usbd_hidFunctions

- \ref USBD_HIDn_Initialize  &mdash; \copybrief USBD_HIDn_Initialize
- \ref USBD_HIDn_Uninitialize  &mdash; \copybrief USBD_HIDn_Uninitialize
- \ref USBD_HIDn_GetReport  &mdash; \copybrief USBD_HIDn_GetReport
- \ref USBD_HIDn_SetReport  &mdash; \copybrief USBD_HIDn_SetReport
- \ref USBD_HID_GetReportTrigger  &mdash; \copybrief USBD_HID_GetReportTrigger

\ref  usbd_mscFunctions

- \ref USBD_MSCn_Initialize  &mdash; \copybrief USBD_MSCn_Initialize
- \ref USBD_MSCn_Uninitialize  &mdash; \copybrief USBD_MSCn_Uninitialize
- \ref USBD_MSCn_GetCacheInfo  &mdash; \copybrief USBD_MSCn_GetCacheInfo
- \ref USBD_MSCn_GetMediaCapacity  &mdash; \copybrief USBD_MSCn_GetMediaCapacity
- \ref USBD_MSCn_Read  &mdash; \copybrief USBD_MSCn_Read
- \ref USBD_MSCn_Write  &mdash; \copybrief USBD_MSCn_Write
- \ref USBD_MSCn_CheckMedia  &mdash; \copybrief USBD_MSCn_CheckMedia
- \ref USBD_MSCn_GetMaxLUN  &mdash; \copybrief USBD_MSCn_GetMaxLUN
- \ref USBD_MSCn_LUN_GetMediaCapacity  &mdash; \copybrief USBD_MSCn_LUN_GetMediaCapacity
- \ref USBD_MSCn_LUN_Read  &mdash; \copybrief USBD_MSCn_LUN_Read
- \ref USBD_MSCn_LUN_Write  &mdash; \copybrief USBD_MSCn_LUN_Write
- \ref USBD_MSCn_LUN_CheckMedia  &mdash; \copybrief USBD_MSCn_LUN_CheckMedia
- \ref USBD_MSCn_SetMediaOwnerUSB  &mdash; \copybrief USBD_MSCn_SetMediaOwnerUSB
- \ref USBD_MSCn_SetMediaOwnerFS  &mdash; \copybrief USBD_MSCn_SetMediaOwnerFS

## USB Host {#usbh_functions}

\ref  usbh_coreFunctions

- \ref USBH_GetVersion &mdash; \copybrief USBH_GetVersion
- \ref USBH_Initialize  &mdash; \copybrief USBH_Initialize
- \ref USBH_Uninitialize  &mdash; \copybrief USBH_Uninitialize
- \ref USBH_Port_Suspend  &mdash; \copybrief USBH_Port_Suspend
- \ref USBH_Port_Resume  &mdash; \copybrief USBH_Port_Resume
- \ref USBH_Device_GetController  &mdash; \copybrief USBH_Device_GetController
- \ref USBH_Device_GetPort  &mdash; \copybrief USBH_Device_GetPort
- \ref USBH_Device_GetStatus  &mdash; \copybrief USBH_Device_GetStatus
- \ref USBH_Device_GetSpeed  &mdash; \copybrief USBH_Device_GetSpeed
- \ref USBH_Device_GetAddress  &mdash; \copybrief USBH_Device_GetAddress
- \ref USBH_Device_GetVID  &mdash; \copybrief USBH_Device_GetVID
- \ref USBH_Device_GetPID  &mdash; \copybrief USBH_Device_GetPID
- \ref USBH_Device_GetStringDescriptor  &mdash; \copybrief USBH_Device_GetStringDescriptor
- \ref USBH_Notify  &mdash; \copybrief USBH_Notify

\ref  usbh_cdcacmFunctions

- \ref USBH_CDC_ACM_GetDevice  &mdash; \copybrief USBH_CDC_ACM_GetDevice
- \ref USBH_CDC_ACM_GetLineCoding  &mdash; \copybrief USBH_CDC_ACM_GetLineCoding
- \ref USBH_CDC_ACM_GetRxCount  &mdash; \copybrief USBH_CDC_ACM_GetRxCount
- \ref USBH_CDC_ACM_GetStatus  &mdash; \copybrief USBH_CDC_ACM_GetStatus
- \ref USBH_CDC_ACM_GetTxCount  &mdash; \copybrief USBH_CDC_ACM_GetTxCount
- \ref USBH_CDC_ACM_Initialize  &mdash; \copybrief USBH_CDC_ACM_Initialize
- \ref USBH_CDC_ACM_Notify  &mdash; \copybrief USBH_CDC_ACM_Notify
- \ref USBH_CDC_ACM_Receive  &mdash; \copybrief USBH_CDC_ACM_Receive
- \ref USBH_CDC_ACM_AbortReceive  &mdash; \copybrief USBH_CDC_ACM_AbortReceive
- \ref USBH_CDC_ACM_Send  &mdash; \copybrief USBH_CDC_ACM_Send
- \ref USBH_CDC_ACM_AbortSend  &mdash; \copybrief USBH_CDC_ACM_AbortSend
- \ref USBH_CDC_ACM_SendBreak  &mdash; \copybrief USBH_CDC_ACM_SendBreak
- \ref USBH_CDC_ACM_SetControlLineState  &mdash; \copybrief USBH_CDC_ACM_SetControlLineState
- \ref USBH_CDC_ACM_SetLineCoding  &mdash; \copybrief USBH_CDC_ACM_SetLineCoding
- \ref USBH_CDC_ACM_Uninitialize  &mdash; \copybrief USBH_CDC_ACM_Uninitialize

\ref  usbh_hidFunctions

- \ref USBH_HID_DataReceived  &mdash; \copybrief USBH_HID_DataReceived
- \ref USBH_HID_GetDevice  &mdash; \copybrief USBH_HID_GetDevice
- \ref USBH_HID_GetKeyboardKey  &mdash; \copybrief USBH_HID_GetKeyboardKey
- \ref USBH_HID_GetMouseState  &mdash; \copybrief USBH_HID_GetMouseState
- \ref USBH_HID_GetStatus  &mdash; \copybrief USBH_HID_GetStatus
- \ref USBH_HID_Initialize  &mdash; \copybrief USBH_HID_Initialize
- \ref USBH_HID_ParseReportDescriptor  &mdash; \copybrief USBH_HID_ParseReportDescriptor
- \ref USBH_HID_Read  &mdash; \copybrief USBH_HID_Read
- \ref USBH_HID_Uninitialize  &mdash; \copybrief USBH_HID_Uninitialize
- \ref USBH_HID_Write  &mdash; \copybrief USBH_HID_Write

\ref  usbh_mscFunctions

- \ref USBH_MSC_Initialize  &mdash; \copybrief USBH_MSC_Initialize
- \ref USBH_MSC_Uninitialize  &mdash; \copybrief USBH_MSC_Uninitialize
- \ref USBH_MSC_GetDevice  &mdash; \copybrief USBH_MSC_GetDevice
- \ref USBH_MSC_GetStatus  &mdash; \copybrief USBH_MSC_GetStatus
- \ref USBH_MSC_Read  &mdash; \copybrief USBH_MSC_Read
- \ref USBH_MSC_Write  &mdash; \copybrief USBH_MSC_Write
- \ref USBH_MSC_ReadCapacity  &mdash; \copybrief USBH_MSC_ReadCapacity

\ref  usbh_customFunctions

- \ref USBH_CustomClass_GetDevice  &mdash; \copybrief USBH_CustomClass_GetDevice
- \ref USBH_CustomClass_GetStatus  &mdash; \copybrief USBH_CustomClass_GetStatus
- \ref USBH_CustomClass_Configure  &mdash; \copybrief USBH_CustomClass_Configure
- \ref USBH_CustomClass_Unconfigure  &mdash; \copybrief USBH_CustomClass_Unconfigure
- \ref USBH_CustomClass_Initialize  &mdash; \copybrief USBH_CustomClass_Initialize
- \ref USBH_CustomClass_Uninitialize  &mdash; \copybrief USBH_CustomClass_Uninitialize
- \ref USBH_PipeCreate  &mdash; \copybrief USBH_PipeCreate
- \ref USBH_PipeRegisterCallback &mdash; \copybrief USBH_PipeRegisterCallback
- \ref USBH_PipeUpdate  &mdash; \copybrief USBH_PipeUpdate
- \ref USBH_PipeDelete  &mdash; \copybrief USBH_PipeDelete
- \ref USBH_PipeReset  &mdash; \copybrief USBH_PipeReset
- \ref USBH_PipeReceive  &mdash; \copybrief USBH_PipeReceive
- \ref USBH_PipeReceiveGetResult  &mdash; \copybrief USBH_PipeReceiveGetResult
- \ref USBH_PipeSend  &mdash; \copybrief USBH_PipeSend
- \ref USBH_PipeSendGetResult  &mdash; \copybrief USBH_PipeSendGetResult
- \ref USBH_PipeAbort  &mdash; \copybrief USBH_PipeAbort
- \ref USBH_ControlTransfer  &mdash; \copybrief USBH_ControlTransfer
- \ref USBH_DeviceRequest_GetStatus  &mdash; \copybrief USBH_DeviceRequest_GetStatus
- \ref USBH_DeviceRequest_ClearFeature  &mdash; \copybrief USBH_DeviceRequest_ClearFeature
- \ref USBH_DeviceRequest_SetFeature  &mdash; \copybrief USBH_DeviceRequest_SetFeature
- \ref USBH_DeviceRequest_SetAddress  &mdash; \copybrief USBH_DeviceRequest_SetAddress
- \ref USBH_DeviceRequest_GetDescriptor  &mdash; \copybrief USBH_DeviceRequest_GetDescriptor
- \ref USBH_DeviceRequest_SetDescriptor  &mdash; \copybrief USBH_DeviceRequest_SetDescriptor
- \ref USBH_DeviceRequest_GetConfiguration  &mdash; \copybrief USBH_DeviceRequest_GetConfiguration
- \ref USBH_DeviceRequest_SetConfiguration  &mdash; \copybrief USBH_DeviceRequest_SetConfiguration
- \ref USBH_DeviceRequest_GetInterface  &mdash; \copybrief USBH_DeviceRequest_GetInterface
- \ref USBH_DeviceRequest_SetInterface  &mdash; \copybrief USBH_DeviceRequest_SetInterface
- \ref USBH_DeviceRequest_SynchFrame  &mdash; \copybrief USBH_DeviceRequest_SynchFrame
