import SwiftUI

struct Signup: View {
    @State private var fullName: String = ""
    @State private var index: String = ""
    @State private var batchName: String = ""
    @State private var email: String = ""
    @State private var password: String = ""
    @State private var agreedToTerms: Bool = false
    
    var body: some View {
        VStack(spacing: 20) {
            Spacer()
            
           
            Text("Sign up")
                .font(.title)
                .bold()
            
          
            Text("Welcome !")
                .font(.title2)
                .bold()
            
           
            Group {
                customTextField("Full Name", text: $fullName)
                customTextField("Index", text: $index)
                customTextField("Batch name", text: $batchName)
                customTextField("Email", text: $email)
                SecureField("Select a password", text: $password)
                    .padding()
                    .background(Color.white)
                    .cornerRadius(8)
                    .shadow(radius: 1)
            }
            
            
            HStack(alignment: .top) {
                Button(action: {
                    agreedToTerms.toggle()
                }) {
                    Image(systemName: agreedToTerms ? "checkmark.circle.fill" : "circle")
                        .foregroundColor(.black)
                }
                Text("By creating an account or signing you agree to our **Terms and Conditions**")
                    .font(.footnote)
                    .foregroundColor(.gray)
            }
            .padding(.top, 10)
            .frame(maxWidth: .infinity, alignment: .leading)
            
           
            Button(action: {
               
            }) {
                Text("Submit")
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Color.cyan.opacity(0.6))
                    .foregroundColor(.black)
                    .cornerRadius(8)
            }
            
            Spacer()
        }
        .padding()
        .background(Color(UIColor.systemGray6))
    }
    
    
    @ViewBuilder
    func customTextField(_ placeholder: String, text: Binding<String>) -> some View {
        TextField(placeholder, text: text)
            .padding()
            .background(Color.white)
            .cornerRadius(8)
            .shadow(radius: 1)
    }
}

#Preview {
    Signup()
}
