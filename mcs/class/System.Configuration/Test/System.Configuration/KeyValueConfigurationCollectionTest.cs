//
// System.Configuration.KeyValueConfigurationCollectionTest.cs - Unit tests
// for System.Configuration.KeyValueConfigurationCollection.
//
// Author:
//	Chris Toshok  <toshok@ximian.com>
//
// Copyright (C) 2005 Novell, Inc (http://www.novell.com)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#if NET_2_0

using System;
using System.Configuration;
using NUnit.Framework;

namespace MonoTests.System.Configuration {
	[TestFixture]
	public class KeyValueConfigurationCollectionTest
	{
		class Poker : KeyValueConfigurationCollection
		{
			bool useElementPoker;

			public Poker () : this (false)
			{ }

			public Poker (bool useElementPoker)
			{
				this.useElementPoker = useElementPoker;
			}

			public ConfigurationPropertyCollection GetProperties () {
				return base.Properties;
			}

			public ConfigurationElement DoCreateNewElement () {
				if (useElementPoker)
					return new ElementPoker ("", "");
				else
					return base.CreateNewElement ();
			}

			public object DoGetElementKey (ConfigurationElement element)
			{
				return base.GetElementKey (element);
			}

			public object[] GetAllKeys ()
			{
				return base.BaseGetAllKeys();
			}

			public bool GetThrowOnDuplicate ()
			{
				return base.ThrowOnDuplicate;
			}
		}

		class ElementPoker : KeyValueConfigurationElement
		{
			public ElementPoker (string name, string value)
				: base (name, value)
			{
			}

			protected override void InitializeDefault()
			{
				Console.WriteLine (Environment.StackTrace);
				base.InitializeDefault();
			}

			protected override void Init ()
			{
				//				Console.WriteLine (Environment.StackTrace);
				base.Init();
			}

			public ConfigurationPropertyCollection GetProperties () {
				return base.Properties;
			}
		}

		[Test]
		public void ThrowOnDuplicate ()
		{
			Poker p = new Poker ();
			Assert.IsFalse (p.GetThrowOnDuplicate (), "A1");
		}

		[Test]
		public void Properties ()
		{
			Poker p = new Poker ();
			ConfigurationPropertyCollection props = p.GetProperties();

			Assert.IsNotNull (props, "A1");
			Assert.AreEqual (0, props.Count, "A2");
		}

		[Test]
		public void CreateNewElement ()
		{
			Poker p = new Poker ();
			ConfigurationElement e = p.DoCreateNewElement ();

			Assert.AreEqual (typeof (KeyValueConfigurationElement), e.GetType(), "A1");

			KeyValueConfigurationElement kv = (KeyValueConfigurationElement)e;

			Assert.AreEqual ("", kv.Key, "A2");
			Assert.AreEqual ("", kv.Value, "A3");
		}

		[Test]
		public void Add ()
		{
			Poker p = new Poker (true);
			ElementPoker ep = new ElementPoker ("", "");

			p.Add (ep);
		}


		[Test]
		public void AddDuplicate ()
		{
			Poker p = new Poker (true);
			ElementPoker ep;

			ep = new ElementPoker ("hi", "bye");
			p.Add (ep);

			ep = new ElementPoker ("hi", "bye2");
			p.Add (ep);

			Assert.AreEqual (1, p.AllKeys.Length, "A1");
			Assert.AreEqual (1, p.GetAllKeys().Length, "A2");
		}

		[Test]
		public void GetElementKey()
		{
			Poker p = new Poker ();
			ElementPoker ep = new ElementPoker ("foo", "there");
			p.Add (ep);
			Assert.AreEqual ("foo", p.DoGetElementKey (ep), "A1");
		}

		[Test]
		[Category ("NotWorking")]
		public void GetElementKey_withoutAdd()
		{
			Poker p = new Poker ();
			ElementPoker ep = new ElementPoker ("hi", "there");
			Assert.AreEqual ("", p.DoGetElementKey (ep), "A1");
		}

		[Test]
		[ExpectedException (typeof (NullReferenceException))]
		public void GetElementKey_null ()
		{
			Poker p = new Poker ();
			Assert.AreEqual ("", p.DoGetElementKey (null), "A1");
		}
	}

}

#endif
