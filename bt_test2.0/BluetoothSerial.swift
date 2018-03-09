//
//  BluetoothSerial.swift (originally DZBluetoothSerialHandler.swift)
//  HM10 Serial
//
//  Created by Alex on 09-08-15.
//  Copyright (c) 2017 Hangar42. All rights reserved.
//
//

import UIKit
import CoreBluetooth

/// 全局串行處理程序，記得用init(delgate)去初始化
/// Global serial handler, don't forget to initialize it with init(delgate:)
var serial: BluetoothSerial!

// 委託的函式
// Delegate functions
protocol BluetoothSerialDelegate
{
    // ** Required **
    
    ///當CBCentralManager物件的狀態改變時（例如當藍牙打開/關閉時）被調用
    /// Called when de state of the CBCentralManager changes (e.g. when bluetooth is turned on/off)
    func serialDidChangeState()
    
    ///當外設斷開連接時調用
    /// Called when a peripheral disconnected
    func serialDidDisconnect(_ peripheral: CBPeripheral, error: NSError?)
    
    // 可選
    // ** Optionals **
    
    ///當一個字串訊息被收到的時後調用
    /// Called when a message is received
    func serialDidReceiveString(_ message: String)
    
    ///當一個非負數8-bite字元值型態的訊息被接收到的時候去呼叫
    /// Called when a message is received
    func serialDidReceiveBytes(_ bytes: [UInt8])
    
    ///當一個Data型態訊息被接收到的時候去呼叫
    /// Called when a message is received
    func serialDidReceiveData(_ data: Data)
    
    /// 當一個連線中的外部設備的RSSI被讀取的時後去調用
    /// Called when the RSSI of the connected peripheral is read
    func serialDidReadRSSI(_ rssi: NSNumber)
    
    ///在掃描時發現新外設時調用。還給出了RSSI
    /// Called when a new peripheral is discovered while scanning. Also gives the RSSI (signal strength)
    func serialDidDiscoverPeripheral(_ peripheral: CBPeripheral, RSSI: NSNumber?)
    
    /// 當外設連接（但尚未準備好進行通信）時調用
    /// Called when a peripheral is connected (but not yet ready for cummunication)
    func serialDidConnect(_ peripheral: CBPeripheral)
    
    /// 擱置的連線失敗時調用
    /// Called when a pending connection failed
    func serialDidFailToConnect(_ peripheral: CBPeripheral, error: NSError?)
    
    /// 當外設準備好進行通信時調用
    /// Called when a peripheral is ready for communication
    func serialIsReady(_ peripheral: CBPeripheral)
}
// 施做一些委託的函式(可選)
// Make some of the delegate functions optional
extension BluetoothSerialDelegate
{
    func serialDidReceiveString(_ message: String) {}
    func serialDidReceiveBytes(_ bytes: [UInt8]) {}
    func serialDidReceiveData(_ data: Data) {}
    func serialDidReadRSSI(_ rssi: NSNumber) {}
    func serialDidDiscoverPeripheral(_ peripheral: CBPeripheral, RSSI: NSNumber?) {}
    func serialDidConnect(_ peripheral: CBPeripheral) {}
    func serialDidFailToConnect(_ peripheral: CBPeripheral, error: NSError?) {}
    func serialIsReady(_ peripheral: CBPeripheral) {}
}

//fina用法：參考http://swifter.tips/final/
final class BluetoothSerial: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    // MARK: Variables
    
    /// 委託的物件將會調用BluetoothDelegate方法。
    /// The delegate object the BluetoothDelegate methods will be called upon
    var delegate: BluetoothSerialDelegate!
    
    /// 這個藍牙串行處理程序用於CBCentralManager
    /// The CBCentralManager this bluetooth serial handler uses for... well, everything really
    var centralManager: CBCentralManager!
    
    /// 我們正在嘗試連接的外設（如果沒有，則為零）
    /// The peripheral we're trying to connect to (nil if none)
    var pendingPeripheral: CBPeripheral?
    
    ///連接的外設（如果沒有連接的話，為零)
    /// The connected peripheral (nil if none is connected)
    var connectedPeripheral: CBPeripheral?
 
    ///我們需要寫的特徵0xFFE1，連接的外圍設備
    /// The characteristic 0xFFE1 we need to write to, of the connectedPeripheral
    weak var writeCharacteristic: CBCharacteristic?
    
    ///這個串口是否準備好發送和接收數據
    /// Whether this serial is ready to send and receive data
    var isReady: Bool
    {
        get
        {
            return centralManager.state == .poweredOn &&
                   connectedPeripheral != nil &&
                   writeCharacteristic != nil
        }
    }
    
    
    ///這個系列是否在尋找廣告外設
    /// Whether this serial is looking for advertising peripherals
    var isScanning: Bool
    {
        return centralManager.isScanning
    }
    
    
    ///中央管理器的狀態是否為開啟的狀態
    /// Whether the state of the centralManager is .poweredOn
    var isPoweredOn: Bool
    {
        return centralManager.state == .poweredOn
    }
    
    ///要查找的服務的UUID。
    /// UUID of the service to look for.
    var serviceUUID = CBUUID(string: "FFE0")
    
    /// 特徵值的UUID要尋找。
    /// UUID of the characteristic to look for.
    var characteristicUUID = CBUUID(string: "FFE1")
    
    ///寫入HM10是否有或沒有回應。自動設定
    /// Whether to write to the HM10 with or without response. Set automatically.
    /// Legit HM10 modules (from JNHuaMao) require 'Write without Response',
    /// while fake modules (e.g. from Bolutek) require 'Write with Response'.
    private var writeType: CBCharacteristicWriteType = .withoutResponse
    
    
    // MARK: functions
    
    ///始終使用它來初始化一個實例
    /// Always use this to initialize an instance
    init(delegate: BluetoothSerialDelegate)
    {
        super.init()
        self.delegate = delegate
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    /// 開始掃描外設
    /// Start scanning for peripherals
    func startScan()
    {
        guard centralManager.state == .poweredOn else { return }
        
        
        // 開始使用正確的服務UUID掃描外設
        // start scanning for peripherals with correct service UUID
        centralManager.scanForPeripherals(withServices: [serviceUUID], options: nil)
        
        //檢索已連接的外圍設備
        // retrieve peripherals that are already connected
        // see this stackoverflow question http://stackoverflow.com/questions/13286487
        let peripherals = centralManager.retrieveConnectedPeripherals(withServices: [serviceUUID])
        for peripheral in peripherals
        {
            delegate.serialDidDiscoverPeripheral(peripheral, RSSI: nil)
        }
    }
    
    
    /// 停止掃描外設
    /// Stop scanning for peripherals
    func stopScan() {
        centralManager.stopScan()
    }
    
    ///嘗試連接到指定的外設
    /// Try to connect to the given peripheral
    func connectToPeripheral(_ peripheral: CBPeripheral)
    {
        pendingPeripheral = peripheral
        centralManager.connect(peripheral, options: nil)
    }
    
    ///斷開連接的外圍設備或停止連接
    /// Disconnect from the connected peripheral or stop connecting to it
    func disconnect()
    {
        if let p = connectedPeripheral
        {
            centralManager.cancelPeripheralConnection(p)
        }
        else if let p = pendingPeripheral
        {
            centralManager.cancelPeripheralConnection(p) //TODO: Test whether its neccesary to set p to nil
        }
    }
    
    ///在調用此函數後，將調用didReadRSSI delegate function這個委託函式
    /// The didReadRSSI delegate function will be called after calling this function
    func readRSSI()
    {
        guard isReady else { return }
        connectedPeripheral!.readRSSI()
    }
    
    /// 發送字符串到設備
    /// Send a string to the device
    func sendMessageToDevice(_ message: String)
    {
        guard isReady else { return }
        
      
        if let data = message.data(using: String.Encoding.utf8)
        {
            
            connectedPeripheral!.writeValue(data, for: writeCharacteristic!, type: writeType)
        }
    }
    
    /// 發送字元陣列到設備
    /// Send an array of bytes to the device
    func sendBytesToDevice(_ bytes: [UInt8])
    {
        guard isReady else { return }
        
        let data = Data(bytes: UnsafePointer<UInt8>(bytes), count: bytes.count)
        connectedPeripheral!.writeValue(data, for: writeCharacteristic!, type: writeType)
    }
    
    
    
    
    ///將數據發送到設備
    /// Send data to the device
    
    func addDataToDevice(_ data: Data)
    {
        guard isReady else { return }
        
        connectedPeripheral!.writeValue(data, for: writeCharacteristic!, type: CBCharacteristicWriteType.withResponse)
    }
    
    ///將數據發送到設備
    /// Send data to the device
    
    func sendDataToDevice(_ data: Data)
    {
        guard isReady else { return }
        
        connectedPeripheral!.writeValue(data, for: writeCharacteristic!, type: writeType)
    }
    
    
    // MARK: CBCentralManagerDelegate functions

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber)
    {
        // just send it to the delegate
        delegate.serialDidDiscoverPeripheral(peripheral, RSSI: RSSI)
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral)
    {
        //設置一些東西吧
        // set some stuff right
        peripheral.delegate = self
        pendingPeripheral = nil
        connectedPeripheral = peripheral
        
        // send it to the delegate
        delegate.serialDidConnect(peripheral)
        
        //好的，外設是連接的，但我們還沒有準備好！
        // Okay, the peripheral is connected but we're not ready yet!
        //首先得到0xFFE0服務
        // First get the 0xFFE0 service
        //然後獲取此服務的0xFFE1特徵
        // Then get the 0xFFE1 characteristic of this service
        //訂閱它，並創建一個弱參考且稍候寫它
        // Subscribe to it & create a weak reference to it (for writing later on),
        //並通過查看特徵屬性找出writeType。
        // and find out the writeType by looking at characteristic.properties.
        //只有這樣，我們才能做好溝通
        // Only then we're ready for communication

        peripheral.discoverServices([serviceUUID])
    }

    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?)
    {
        connectedPeripheral = nil
        pendingPeripheral = nil

        
        //將其發送給委託者
        // send it to the delegate
        delegate.serialDidDisconnect(peripheral, error: error as NSError?)
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?)
    {
        pendingPeripheral = nil

        //將它發送給代表
        // just send it to the delegate
        delegate.serialDidFailToConnect(peripheral, error: error as NSError?)
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager)
    {
        //注意，如果BLE在連接時關閉，則不會調用“didDisconnectPeripheral”
        // note that "didDisconnectPeripheral" won't be called if BLE is turned off while connected
        connectedPeripheral = nil
        pendingPeripheral = nil

        //將它發送給代表
        // send it to the delegate
        delegate.serialDidChangeState()
    }
    
    
    // MARK: CBPeripheralDelegate functions
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?)
    {
        // 發現所有服務的0xFFE1特性（雖然應該只有一個）
        // discover the 0xFFE1 characteristic for all services (though there should only be one)
        for service in peripheral.services!
        {
            peripheral.discoverCharacteristics([characteristicUUID], for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?)
    {
        
        //檢查我們正在尋找的特徵（0xFFE1）是否存在 - 只是為了確定
        // check whether the characteristic we're looking for (0xFFE1) is present - just to be sure
        for characteristic in service.characteristics!
        {
            if characteristic.uuid == characteristicUUID
            {
                
                //訂閱這個值（所以當我們有串口數據時我們會收到通知..）
                // subscribe to this value (so we'll get notified when there is serial data for us..)
                peripheral.setNotifyValue(true, for: characteristic)
                
                //保留對這個特徵的引用，以便我們寫給它
                // keep a reference to this characteristic so we can write to it
                writeCharacteristic = characteristic
                
                //找出writeType
                // find out writeType
                writeType = characteristic.properties.contains(.write) ? .withResponse : .withoutResponse
                
                //通知代表我們準備好進行溝通
                // notify the delegate we're ready for communication
                delegate.serialIsReady(peripheral)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?)
    {
        
        //以不同的方式通知委託
        // notify the delegate in different ways
        //如果你不使用其中的一個，只是評論（為了最佳效率：]）
        // if you don't use one of these, just comment it (for optimum efficiency :])
        let data = characteristic.value
        guard data != nil else { return }
        
        //首先是數據
        // first the data
        delegate.serialDidReceiveData(data!)
        
        //然後是字符串
        // then the string
        if let str = String(data: data!, encoding: String.Encoding.utf8)
        {
            delegate.serialDidReceiveString(str)
        } else
        {
            //取消註釋是為了用於除錯
            //print("Received an invalid string!") uncomment for debugging
        }
        
        //現在的字元陣列
        // now the bytes array
        var bytes = [UInt8](repeating: 0, count: data!.count / MemoryLayout<UInt8>.size)
        (data! as NSData).getBytes(&bytes, length: data!.count)
        delegate.serialDidReceiveBytes(bytes)
    }
    
    func peripheral(_ peripheral: CBPeripheral, didReadRSSI RSSI: NSNumber, error: Error?) {
        delegate.serialDidReadRSSI(RSSI)
    }
}
