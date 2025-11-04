import SwiftUI
import FirebaseDatabase

struct AirConditionView: View {
    @State private var isOn: Bool = true
    @State private var temperature: Int = 24
    @State private var isCoolSelected = true
    @State private var isHotSelected = false
    @State private var isAutoSelected = false
    @State private var timer: String = "15:30"
    @State private var fanSpeed: Int = 2

    private var ref: DatabaseReference = Database.database().reference()

    var body: some View {
        VStack(spacing: 20) {
            HStack {
                Text("Air Condition")
                    .font(.title2)
                    .bold()
                Spacer()
                Toggle("", isOn: $isOn)
                    .labelsHidden()
                    .onChange(of: isOn) { newValue in
                        sendToFirebase()
                    }
            }
            .padding()

            VStack {
                Circle()
                    .stroke(Color.blue, lineWidth: 8)
                    .frame(width: 180, height: 180)
                    .overlay(
                        VStack {
                            Text("62%")
                                .font(.caption)
                                .foregroundColor(.gray)
                            Text("\(temperature)°C")
                                .font(.largeTitle)
                                .bold()
                        }
                    )

                HStack(spacing: 30) {
                    Button(action: {
                        temperature -= 1
                        sendToFirebase()
                    }) {
                        Image(systemName: "minus.circle")
                            .font(.largeTitle)
                    }

                    Button(action: {
                        temperature += 1
                        sendToFirebase()
                    }) {
                        Image(systemName: "plus.circle")
                            .font(.largeTitle)
                    }
                }
                .padding(.top)
            }

            HStack(spacing: 20) {
                Button("Cool") {
                    isCoolSelected = true
                    isHotSelected = false
                    isAutoSelected = false
                    sendToFirebase()
                }
                .buttonStyle(ModeButtonStyle(isSelected: isCoolSelected))

                Button("Hot") {
                    isCoolSelected = false
                    isHotSelected = true
                    isAutoSelected = false
                    sendToFirebase()
                }
                .buttonStyle(ModeButtonStyle(isSelected: isHotSelected))

                Button("Auto") {
                    isCoolSelected = false
                    isHotSelected = false
                    isAutoSelected = true
                    sendToFirebase()
                }
                .buttonStyle(ModeButtonStyle(isSelected: isAutoSelected))
            }

            HStack {
                VStack {
                    Text("Timer")
                        .font(.subheadline)
                    TextField("Timer", text: $timer)
                        .font(.title3)
                        .bold()
                        .multilineTextAlignment(.center)
                        .keyboardType(.numbersAndPunctuation)
                        .onChange(of: timer) { _ in sendToFirebase() }
                }
                .padding()
                .background(Color.white)
                .cornerRadius(15)
                .shadow(radius: 5)

                VStack {
                    Text("Fan Speed")
                        .font(.subheadline)
                    Stepper(value: $fanSpeed, in: 1...5) {
                        Text("\(fanSpeed)")
                            .font(.title3)
                            .bold()
                    }
                    .onChange(of: fanSpeed) { _ in sendToFirebase() }
                }
                .padding()
                .background(Color.white)
                .cornerRadius(15)
                .shadow(radius: 5)
            }

            Spacer()
        }
        .padding()
        .background(Color(.systemGray6))
        .onAppear {
            loadFromFirebase()
        }
    }

    func currentMode() -> String {
        if isCoolSelected { return "cool" }
        if isHotSelected { return "hot" }
        return "auto"
    }

    func sendToFirebase() {
        let data: [String: Any] = [
            "isOn": isOn,
            "temperature": temperature,
            "mode": currentMode(),
            "timer": timer,
            "fanSpeed": fanSpeed
        ]
        ref.child("airConditioner").setValue(data)
    }

    func loadFromFirebase() {
        ref.child("airConditioner").observeSingleEvent(of: .value) { snapshot in
            if let value = snapshot.value as? [String: Any] {
                self.isOn = value["isOn"] as? Bool ?? true
                self.temperature = value["temperature"] as? Int ?? 24
                self.timer = value["timer"] as? String ?? "15:30"
                self.fanSpeed = value["fanSpeed"] as? Int ?? 2
                if let mode = value["mode"] as? String {
                    isCoolSelected = (mode == "cool")
                    isHotSelected = (mode == "hot")
                    isAutoSelected = (mode == "auto")
                }
            }
        }
    }
}

struct ModeButtonStyle: ButtonStyle {
    var isSelected: Bool

    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding()
            .frame(width: 80)
            .background(isSelected ? Color.blue : Color(.systemGray5))
            .foregroundColor(isSelected ? .white : .black)
            .cornerRadius(10)
    }
}

#Preview {
    AirConditionView()
}
