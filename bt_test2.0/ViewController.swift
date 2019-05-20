//
//  ViewController.swift
//  bt_test2.0
//
//  Created  on 2017/9/11.
//

import UIKit
import CoreBluetooth

extension String {
    subscript(value: PartialRangeUpTo<Int>) -> Substring {
        get {
            return self[..<index(startIndex, offsetBy: value.upperBound)]
        }
    }
    
    subscript(value: PartialRangeThrough<Int>) -> Substring {
        get {
            return self[...index(startIndex, offsetBy: value.upperBound)]
        }
    }
    
    subscript(value: PartialRangeFrom<Int>) -> Substring {
        get {
            return self[index(startIndex, offsetBy: value.lowerBound)...]
        }
    }
}
extension Data {
    func hex(separator:String = "") -> String {
        return (self.map { String(format: "%02X", $0) }).joined(separator: separator)
    }
}

extension Decimal {
    var uint16Value:UInt16
    {
        return NSDecimalNumber(decimal: self).uint16Value
    }
}




class ViewController: UIViewController, BluetoothSerialDelegate

{
//MARK: IBOutlets
    @IBOutlet weak var txtInfo: UITextView!
    @IBOutlet weak var myText: UITextView!

    
var martixColor = [[Int]](repeating: [Int](repeating: 0, count: 16), count: 16)

    var line = [UInt8]()
    var totalLine = [UInt8]()
    
//MARK: IBActions
    
    
    @IBAction func btnAutomaticallyFlared(_ sender: UIButton)
    {
        
        
        let uiText = myText.text
        
       // let uiTextLen = myText.text.lengthOfBytes(using: .utf8) / 3
        let uiTextLen = Array(uiText!).count
    print ("***")
        print(uiTextLen)
     
    //   totalLine.removeAll()
        for x in 0 ..< uiTextLen
        {
            totalLine.removeAll()
            
            let singleText = String(Array(uiText!)[x])
            totalLine.append(contentsOf: getText(inpText: singleText))
            print (">>>\(singleText),\(totalLine)")
      
        print ("---")
        print (totalLine)
          //   }
      //  var sendString0 = "SetMsg="
         
            let sendString0 = (x == 0 ? "SetMsg=" : "AddMsg=")
            let newData0 = sendString0.data(using: String.Encoding.utf8)

     //   var c0 = newData0
            let size : UInt8 = 2
            let dataArray :UInt8 = UInt8(totalLine.count)
            let total:Int16 = Int16(size + dataArray)
            let highByte:UInt8 = UInt8(total >> 8)
        let lowByte:UInt8 = UInt8(total & 0xff)
        
            let emptyNSdata = NSMutableData()
        
        //key command
        emptyNSdata.append(newData0!)
        
            let endMarker = NSData(bytes: [UInt8(highByte), lowByte] , length: 2)
        
        emptyNSdata.append(endMarker as Data)
        
//        var dataBytes = NSData(bytes: line, length:  32)
            let dataBytes = NSData(bytes: totalLine, length:  totalLine.count)

        emptyNSdata.append(dataBytes as Data)
        
   
        
        print (">:")
        print (emptyNSdata)
        print ("-:")
        
       
        serial.addDataToDevice( emptyNSdata as Data )
 
    }
    
  }

    @IBAction func btnUpFlared(_ sender: UIButton)    //close
    {
        serial.sendMessageToDevice("c")
    }
    
    @IBAction func btnDownFlared(_ sender: UIButton)
    {
        serial.sendMessageToDevice("o")
    }
    
    @IBAction func btnLeftFlared(_ sender: UIButton)    //opne
    {
        serial.sendMessageToDevice("o")
    }
    
    @IBAction func btnRightFlared(_ sender: UIButton)
    {
        serial.sendMessageToDevice("C")
    }
    
    @IBAction func btnStopFlared(_ sender: UIButton)
    {
        serial.sendMessageToDevice("z")  //藍芽傳送字串“z"
    }
    
    
    func getText (inpText:String) -> [UInt8]
    {
    
        let myView  = ShapeView(frame:CGRect(x:25,y:200,width:320,height:16),text: inpText)
        myView.backgroundColor = UIColor.white
        view.addSubview(myView)
        
        var myColor : Float
        
        
        for i in 0..<16
        {
            for j in 0..<16
            {
                print (myView.layer.colorOfPoint(point: CGPoint(x: i, y: j) ))
                myColor = Float(myView.layer.colorOfPoint(point: CGPoint(x: i, y: j)).components![0])
                
                print ("color:\(myColor)")
                
                if myColor >= 0.5
                {
                    martixColor[i][j] = 0
                    
                }
                else
                {
                    martixColor[i][j] = 1
                }
                
            }
        }
        
        
        //        var line = [Int]()
        line.removeAll()
        for l in 0..<16
        {
            
            var sum : Decimal
            sum = 0
            for k in 0..<16
            {
                
                if martixColor[k][l] == 0
                {
                    print("_", terminator:"")
                }
                else
                {
                    sum +=  (pow(2, 15-k))
                    print("1", terminator:"")
                }
                // print (sum)
                
            }
            
            
            line.append( UInt8((sum.uint16Value >> 8)&0xff) )
            
            line.append(UInt8(sum.uint16Value & 0xff))
            
            
            print (",")
        }
        
        for o in 0..<32
        {
            print ("%02x",line[o])
        }
        
        return line
    }
    
    func showText ()
    {
        let sText =  myText.text
        let myView  = ShapeView(frame:CGRect(x:25,y:200,width:320,height:16),text: sText!)
        myView.backgroundColor = UIColor.white
        view.addSubview(myView)
        
        var myColor : Float
        
        // var martixColor = [[Int]]()
        
        
        
        
        for i in 0..<320
        {
            for j in 0..<16
            {
                print (myView.layer.colorOfPoint(point: CGPoint(x: i, y: j) ))
                myColor = Float(myView.layer.colorOfPoint(point: CGPoint(x: i, y: j)).components![0])
                
                print ("color:\(myColor)")
                
                if myColor >= 0.5
                {
                    martixColor[i][j] = 0
                    
                }
                else
                {
                    martixColor[i][j] = 1
                }
                
            }
        }
        
        
//        var line = [Int]()
        line.removeAll()
        for l in 0..<16
        {
            
            var sum : Decimal
            sum = 0
            for k in 0..<16
            {
                
                if martixColor[k][l] == 0
                {
                    print("_", terminator:"")
                }
                else
                {
                    sum +=  (pow(2, 15-k))
                    print("1", terminator:"")
                }
                // print (sum)
                
            }
           
            
               line.append( UInt8((sum.uint16Value >> 8)&0xff) )
            
               line.append(UInt8(sum.uint16Value & 0xff))
       
            
            print (",")
        }
        
        for o in 0..<32
        {
            print ("%02x",line[o])
        }
        
    }
//MARK: Variables
    
    /// The peripherals that have been discovered (no duplicates and sorted by asc RSSI)
    var peripherals: [(peripheral: CBPeripheral, RSSI: Float)] = []
    
    /// The peripheral the user has selected
    var selectedPeripheral: CBPeripheral?
    
    /// Progress hud shown
    var progressHUD: MBProgressHUD?
    
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
   
        
        serial = BluetoothSerial(delegate: self)
        
        // tell the delegate to notificate US instead of the previous view if something happens
        serial.delegate = self
        
        if serial.centralManager.state == .poweredOn
        {
            txtInfo.text! = "Bluetooth turned on"
//            return
        }

    }
    deinit
    {
        NotificationCenter.default.removeObserver(self)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
//MARK: BluetoothSerialDelegate
    
    func serialDidReceiveString(_ message: String)
    {


    }
    
    func serialDidDisconnect(_ peripheral: CBPeripheral, error: NSError?)
    {

    }
    
    func serialDidChangeState()
    {
        var msg = ""
        
        switch serial.centralManager.state
        {
        case .poweredOff:
            msg = "bluetooth is off"
        case .poweredOn:
            msg = "bluetooth is on"
        case .unsupported:
            msg = "bluetooth is unsupported"
        default:
            break
        }
        print("START:\(msg)")
    }

}

