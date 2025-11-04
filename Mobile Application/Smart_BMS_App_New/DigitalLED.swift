import SwiftUI
import FirebaseDatabase

struct DigitalLEDView: View {
    @State private var isLightOn = true
    let ref = Database.database().reference()
    
    var body: some View {
        ZStack {
            Color(.systemGray6)
                .ignoresSafeArea()
            
            VStack(spacing: 40) {
                Text("Conference Room")
                    .font(.largeTitle.bold())
                
                ZStack {
                   
                    Circle()
                        .fill(isLightOn ? Color.yellow.opacity(0.7) : Color.gray.opacity(0.3))
                        .frame(width: 250, height: 250)
                        .shadow(color: isLightOn ? Color.yellow.opacity(0.6) : Color.clear, radius: isLightOn ? 20 : 0)
                        .scaleEffect(isLightOn ? 1.05 : 1.0)
                        .animation(.easeInOut(duration: 0.3), value: isLightOn)
                        .onTapGesture {
                            isLightOn.toggle()
                            sendLightState(isOn: isLightOn)
                        }
                    
                    Image(systemName: isLightOn ? "lightbulb.fill" : "lightbulb")
                        .resizable()
                        .scaledToFit()
                        .frame(width: 120, height: 120)
                        .foregroundColor(isLightOn ? .yellow : .gray)
                }
                
                Text(isLightOn ? "Light ON" : "Light OFF")
                    .font(.title.bold())
            }
        }
        .onAppear {
            loadLightState()
        }
    }
    
    
    func sendLightState(isOn: Bool) {
        ref.child("digitalLED/state").setValue(isOn) { error, _ in
            if let error = error {
                print("Failed to update light state: \(error.localizedDescription)")
            } else {
                print("Digital LED state updated: \(isLightOn ? "ON" : "OFF")")
            }
        }
    }
    
    func loadLightState() {
        ref.child("digitalLED/state").observeSingleEvent(of: .value) { snapshot in
            if let value = snapshot.value as? Bool {
                isLightOn = value
            }
        }
    }
}

struct DigitalLEDView_Previews: PreviewProvider {
    static var previews: some View {
        DigitalLEDView()
    }
}
