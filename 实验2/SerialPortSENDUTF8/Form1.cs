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

namespace SerialPortSENDUTF8
{
    public partial class Form1 : Form
    {
		private SerialPort comport;
		private Boolean sending;
		string Message = "";
		string PortName = "";
		string SendTime = "";
		public Form1()
        {
            InitializeComponent();
        }
		private const Byte head = 249;
		private void Form1_Load(object sender, EventArgs e)
        {
			comport = CreateComport(comport);
			comport.Close();
			sending = false;
		}
		private SerialPort CreateComport(SerialPort port)
		{
			if (port == null)
			{
				port = new SerialPort("COM1",9600, Parity.None, 8, StopBits.One);
			}
			return port;
		}
		private Boolean OpenComport(SerialPort port)
		{
			try
			{
				if ((port != null) && (!port.IsOpen))
				{
					port.Open();
				}
				return true;
			}
			catch (Exception ex)
			{
				System.Windows.Forms.MessageBox.Show(string.Format("出問題啦:{0}", ex.ToString()));
				return false;
			}
		}
		private Byte[] GetSendBuffer(String content)
		{
			Byte[] dataBytes = Encoding.UTF8.GetBytes(content);
			if (dataBytes.Length < 256)
			{
				Byte[] result = new Byte[dataBytes.Length + 2];
				result[0] = head;
				result[1] = Convert.ToByte(dataBytes.Length);
				Array.Copy(dataBytes, 0, result, 2, dataBytes.Length);
				return result;
			}
			else
			{
				throw new OverflowException();
			}
		}
		private void CloseComport(SerialPort port)
		{
			try
			{
				if ((port != null) && (port.IsOpen) && (!sending))
				{
					port.Close();
					System.Windows.Forms.MessageBox.Show("序列埠已關閉");
				}
			}
			catch (Exception ex)
			{
				//這邊你可以自訂發生例外的處理程序
				System.Windows.Forms.MessageBox.Show(string.Format("出問題啦:{0}", ex.ToString()));
			}
		}
		private void SendData(Object port)
		{
			SendTime = "SEND " + DateTime.Now.ToShortDateString()+" "+DateTime.Now.ToShortTimeString()+" "+Message;
			System.Windows.Forms.MessageBox.Show(SendTime+" "+"PortName:"+PortName);
			Byte[] buffer;
			String[] content = new String[] {SendTime};//最後修改
			for (Int32 i = 0; i <= content.Length - 1; i++)
			{
				buffer = GetSendBuffer(content[i]);
				try
				{
					(port as SerialPort).Write(buffer, 0, buffer.Length);
				}
				catch (Exception ex)
				{
					//這邊你可以自訂發生例外的處理程序
					CloseComport((port as SerialPort));
					System.Windows.Forms.MessageBox.Show(string.Format("出問題啦:{0}", ex.ToString()));
				}
				finally
				{
					sending = false;
				}
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
			comport.Open();
			if (!sending && OpenComport(comport))
			{
				Thread t = new Thread(SendData);
				t.IsBackground = true;
				t.Start(comport as Object);
			}
		}

		private void button2_Click(object sender, EventArgs e)
		{
			CloseComport(comport);
		}

		private void PortSetButton_Click(object sender, EventArgs e)
		{
			PortName = PortBox.Text;
			if (!comport.IsOpen) { comport.Close(); }
			comport.PortName = PortName;
		}

		private void MessageEnterButton_Click(object sender, EventArgs e)
		{
			Message = MessageBox.Text;
		}
	}
}
