import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
class email_sender:
    def __init__(self): 
        print(" ")
        #print("Emailer init")
    def send_email(self,alls,subject,message):
        print("Email service start")
        mail_content = message + "\n" + "http://thingsboard.idlab.uantwerpen.be:8080/dashboard/fe3f32c0-053d-11ea-9923-152d43a0a50d?publicId=fbdea2d0-06c4-11ea-9923-152d43a0a50d"
#The mail addresses and password
        sender_addreses = []
        sender_address = 'lampicka92@gmail.com'
        sender_addreses.append(sender_address)
        sender_address = 'jolatadla3@tlen.pl'
        sender_addreses.append(sender_address)
        sender_address = 'ruben.joosen@gmail.com'
        sender_addreses.append(sender_address)
        sender_pass = 'fanikbuyncbzlrln'
        receiver_address = 'lampicka92@gmail.com'
#Setup the MIME
        message = MIMEMultipart()
        sender_address = 'lampicka92@gmail.com'
        message['From'] = sender_address
        message['To'] = receiver_address
        message['Subject'] = subject  #The subject line
#The body and the attachments for the mail
        message.attach(MIMEText(mail_content, 'plain'))
#Create SMTP session for sending the mail
        session = smtplib.SMTP('smtp.gmail.com', 587) #use gmail with port
        session.starttls() #enable security
        session.login(sender_address, sender_pass) #login with mail_id and password
        text = message.as_string()
        if (alls == True):
            for add in sender_addreses:
                session.sendmail(sender_address, add, text)
                print('*** Mail sent ***')
            
            session.quit()
        else:
            sender_address =  'jolatadla3@tlen.pl' #'lampicka92@gmail.com'#'jolatadla3@tlen.pl'
            session.sendmail(sender_address, sender_address, text)
            session.quit()
            print('*** Mail sent ***')
