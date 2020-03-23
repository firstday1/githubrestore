using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;

namespace SerialPortDERCUTF8
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

		private const Byte head = 249;
		private Boolean receiving;
		private SerialPort comport;
		private Int32 totalLength = 0;
		private Thread t;
		delegate void Display(Byte[] buffer);

		private void DisplayText(Byte[] buffer)
		{
			string GetTime = "";
			GetTime = "RECV " + DateTime.Now.ToShortDateString() + DateTime.Now.ToShortTimeString();
			textBox1.Text += GetTime + "\n";
			textBox1.Text += String.Format("{0}{1}", Encoding.UTF8.GetString(buffer), Environment.NewLine);
			totalLength = totalLength + buffer.Length;
			BitNumber.Text = totalLength.ToString();
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			comport = new SerialPort("COM1", 9600, Parity.None, 8, StopBits.One);
			if (!comport.IsOpen)
			{
				comport.Open();
				receiving = true;
				t = new Thread(DoReceive);
				t.IsBackground = true;
				t.Start();
			}
		}

		private void DoReceive()
		{
			List<Byte> tempList = new List<Byte>();
			Byte[] buffer = new Byte[1024];
			Int32 messageDataLength = 0;
			while (receiving)
			{
				Thread.Sleep(100);
				if (comport.BytesToRead > 0)
				{
					Int32 receivedLength = comport.Read(buffer, 0, buffer.Length);
					Array.Resize(ref buffer, receivedLength);
					tempList.AddRange(buffer);
					Array.Resize(ref buffer, 1024);
				}
				if (tempList.Count > 0)
				{
					if (messageDataLength == 0)
					{
						messageDataLength = GetMessageDataLength(tempList);
					}
					else
					{
						messageDataLength = Parse(tempList, messageDataLength);
					}
				}
			}
		}

		private void Button_ClearTextbox_Click(object sender, EventArgs e)
		{
			textBox1.Clear();
		}

		private void Button_AbortThread_Click(object sender, EventArgs e)
		{
			receiving = false;
		}
		private Int32 GetMessageDataLength(List<Byte> tempList)
		{
			if (tempList.Count >= 2)
			{
				Int32 startIndex = tempList.IndexOf(head);
				if (startIndex >= 0 && startIndex < tempList.Count)
				{
					return Convert.ToInt32(tempList[startIndex + 1]);
				}
				else
				{ return 0; }
			}
			else
			{ return 0; }
		}
		private Int32 Parse(List<Byte> tempList, Int32 messageDataLength)
		{
			if (tempList.Count >= messageDataLength)
			{
				Byte[] tempArray = new Byte[messageDataLength];
				Int32 startIndex = tempList.IndexOf(head);
				if (startIndex >= 0)
				{
					tempList.CopyTo(startIndex + 2, tempArray, 0, messageDataLength);
					tempList.RemoveRange(0, startIndex + messageDataLength + 2);
					messageDataLength = GetMessageDataLength(tempList);
					Display d = new Display(DisplayText);
					this.Invoke(d, new Object[] { tempArray });
				}
			}
			return messageDataLength;
		}

		private void button1_Click(object sender, EventArgs e)
		{
			textBox1.Clear();
		}
	}
}
