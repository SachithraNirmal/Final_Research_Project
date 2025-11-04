import SwiftUI
import FirebaseDatabase

struct SmartLightView: View {
    @State private var brightness: Double = 0.47
    @State private var fromTime = Date()
    @State private var toTime = Date()
    @State private var isLightOn = true
    
    let ref = Database.database().reference()
    
    var body: some View {
        VStack(spacing: 20) {
          
            HStack {
                Text("Smart Light")
                    .font(.title2)
                    .bold()
                Spacer()
                Toggle("", isOn: $isLightOn)
                    .labelsHidden()
                    .onChange(of: isLightOn) { newValue in
                        sendLightState(isOn: newValue)
                    }
            }
            .padding()
            
            Image(systemName: isLightOn ? "lightbulb.fill" : "lightbulb")
                .resizable()
                .scaledToFit()
                .frame(height: 150)
                .foregroundColor(isLightOn ? .yellow : .gray)
            
            Text("\(Int(round(brightness * 100)))%")
                .font(.largeTitle)
                .bold()
                .opacity(isLightOn ? 1 : 0.3)
            
            Text("Brightness")
                .font(.subheadline)
                .foregroundColor(.gray)
            
            Slider(value: $brightness, in: 0...1)
                .accentColor(.blue)
                .padding()
                .disabled(!isLightOn)
                .onChange(of: brightness) { newValue in
                   
                    let roundedValue = Double(Int(round(newValue * 100)))
                    sendBrightnessData(brightness: roundedValue)
                }
            
            VStack(alignment: .leading, spacing: 16) {
                Text("Schedule")
                    .font(.headline)
                
                VStack {
                    DatePicker("From", selection: $fromTime, displayedComponents: .hourAndMinute)
                        .datePickerStyle(.compact)
                    DatePicker("To", selection: $toTime, displayedComponents: .hourAndMinute)
                        .datePickerStyle(.compact)
                }
                .padding()
                .background(Color.white)
                .cornerRadius(10)
                .shadow(radius: 2)
            }
            .padding()
            
            Button(action: {
                setScheduleData()
            }) {
                Text("Set Schedule")
                    .bold()
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Color.blue)
                    .foregroundColor(.white)
                    .cornerRadius(10)
            }
            .padding(.horizontal)
            
            Spacer()
        }
        .padding()
        .background(Color(.systemGray6))
        .onAppear {
            loadScheduleAndBrightness()
        }
    }
    
    func sendLightState(isOn: Bool) {
        ref.child("smartLight/state").setValue(isOn) { error, _ in
            if let error = error {
                print("Failed to update light state: \(error.localizedDescription)")
            } else {
                print("💡 Light state updated: \(isOn ? "ON" : "OFF")")
            }
        }
    }
    
    func sendBrightnessData(brightness: Double) {
        ref.child("smartLight/brightness").setValue(brightness) { error, _ in
            if let error = error {
                print("Brightness error: \(error.localizedDescription)")
            } else {
                print("Brightness saved: \(brightness)")
            }
        }
    }
    
    func setScheduleData() {
        let dateFormatter = DateFormatter()
        dateFormatter.timeStyle = .short
        
        let scheduleData: [String: Any] = [
            "scheduleFrom": dateFormatter.string(from: fromTime),
            "scheduleTo": dateFormatter.string(from: toTime)
        ]
        
        ref.child("smartLight/schedule").setValue(scheduleData) { error, _ in
            if let error = error {
                print("Schedule error: \(error.localizedDescription)")
            } else {
                print("Schedule saved")
            }
        }
    }
    
    func loadScheduleAndBrightness() {
        ref.child("smartLight").observeSingleEvent(of: .value) { snapshot in
            if let value = snapshot.value as? [String: Any] {
                if let brightness = value["brightness"] as? Double {
               
                    self.brightness = brightness / 100.0
                }
                if let state = value["state"] as? Bool {
                    self.isLightOn = state
                }
                let formatter = DateFormatter()
                formatter.timeStyle = .short
                
                if let schedule = value["schedule"] as? [String: String] {
                    if let from = schedule["scheduleFrom"],
                       let to = schedule["scheduleTo"],
                       let fromDate = formatter.date(from: from),
                       let toDate = formatter.date(from: to) {
                        self.fromTime = fromDate
                        self.toTime = toDate
                    }
                }
            }
        }
    }
}

struct SmartLightView_Previews: PreviewProvider {
    static var previews: some View {
        SmartLightView()
    }
}
