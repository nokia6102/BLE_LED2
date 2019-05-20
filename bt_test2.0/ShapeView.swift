//
//  ShapeView.swift
//  IOS10DrawShapesTutorial
//
//  Created by Arthur Knopper on 01/10/2016.
//  Copyright © 2016 Arthur Knopper. All rights reserved.
//

import UIKit

extension CALayer {
    
    func colorOfPoint(point:CGPoint) -> CGColor {
        
        var pixel: [CUnsignedChar] = [0, 0, 0, 0]
        
        let colorSpace = CGColorSpaceCreateDeviceRGB()
        let bitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue)
        
        let context = CGContext(data: &pixel, width: 1, height: 1, bitsPerComponent: 8, bytesPerRow: 4, space: colorSpace, bitmapInfo: bitmapInfo.rawValue)
        
        context!.translateBy(x: -point.x, y: -point.y)
        
        self.render(in: context!)
        
        let red: CGFloat   = CGFloat(pixel[0]) / 255.0
        let green: CGFloat = CGFloat(pixel[1]) / 255.0
        let blue: CGFloat  = CGFloat(pixel[2]) / 255.0
        let alpha: CGFloat = CGFloat(pixel[3]) / 255.0
        
        let color = UIColor(red:red, green: green, blue:blue, alpha:alpha)
        
        return color.cgColor
    }
}

class ShapeView: UIView {
    @objc var currentShapeType: Int = 0
    @objc var currentText: String = ""
    @objc init(frame: CGRect, text : String)
    {
        super.init(frame: frame)
        self.currentText = text
     
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
    override func draw(_ rect: CGRect) {
      
        drawText()
    
    }
    //---
    
    
    
    
    @objc func drawText()
    {
        let str = self.currentText
     //   let len = self.currentText.lengthOfBytes(using: .utf8)
        
        let rect = CGRect(origin: CGPoint(x: -1,y :-3), size: CGSize(width: 20, height: 20))      //swift 4
//        let font = UIFont.systemFont(ofSize: 16)
        let fw  = UIFont.Weight.regular
//        var fw  = UIFont.Weight.bold
        let font = UIFont.systemFont(ofSize: 18, weight: fw)
        
        let color = UIColor.black
        let style = NSMutableParagraphStyle()
        
        style.alignment = NSTextAlignment.justified
        
        (str as NSString).draw(in: rect, withAttributes: [NSAttributedString.Key.font:font,NSAttributedString.Key.foregroundColor:color,NSAttributedString.Key.paragraphStyle:style])
    }
    

    
    
    /*
     // Only override draw() if you perform custom drawing.
     // An empty implementation adversely affects performance during animation.
     override func draw(_ rect: CGRect) {
     // Drawing code
     }
     */
    
    
    
}
